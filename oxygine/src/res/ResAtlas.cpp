#include "ResAtlas.h"
#include "core/pixel.h"
#include "core/ImageDataOperations.h"
#include "utils/ImageUtils.h"
#include "MemoryTexture.h"
#include "utils/AtlasTool.h"
#include "Resources.h"
#include "CreateResourceContext.h"
#include "core/NativeTexture.h"
#include <stdio.h>
#include "core/Mem2Native.h"
#include "core/VideoDriver.h"
#include "core/Renderer.h"

#ifdef __S3E__
#include "IwImage.h"
#endif

//#define  LOGD(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)  

namespace oxygine
{
	string getPath(const char *currentPath, const char *str);

	struct atlas_data
	{
		spNativeTexture texture;
		MemoryTexture mt;
		Atlas atlas;
	};


	void apply_atlas(atlas_data &ad)
	{
		if (!ad.texture)
			return;
		

		MemoryTexture mt;
		const Rect &bounds = ad.atlas.getBounds();
		int w = nextPOT(bounds.getWidth());
		int h = nextPOT(bounds.getHeight());
		mt.init(ad.mt.lock().getRect(Rect(0, 0, w, h)));
		
		ImageData image_data = mt.lock();	
		ad.texture->init(image_data, false);
		ad.mt.unlock();

		ad.texture->apply();

		/*
		char str[255];
		sprintf(str, "%d.png", ad.texture.get());
		saveImage(image_data, str, "png");
		*/
	}

	void next_atlas(int w, int h, TextureFormat tf, atlas_data &ad, const char *name = 0)
	{
		ad.mt.init(w, h, tf);
		ad.mt.fill_zero();

		ad.atlas.clean();
		ad.atlas.init(w, h);
		//printf("creating next atlas\n");
		ad.texture = IVideoDriver::instance->createTexture();
		ad.texture->setName(name);
		ad.texture->init(0, w, h, TF_UNDEFINED);
		//ad.texture->init(ad.mt.getWidth(), ad.mt.getHeight(), ad.mt.getFormat());
	}

	void ResAtlas::init_resAnim(ResAnim *rs, const string &file, pugi::xml_node node)
	{
		rs->setName(Resource::extractID(node, file, ""));
		setNode(rs, node);
	}

	void ResAtlas::addAtlas(TextureFormat tf, const string &base, const string &alpha, int w, int h)
	{
		atlas atl;
		atl.base = IVideoDriver::instance->createTexture();
		atl.base_path = base;
		atl.base->setName(base);
		atl.base->init(0, w, h, tf);			
						
		if (!alpha.empty())
		{
			atl.alpha = IVideoDriver::instance->createTexture();
			atl.alpha_path = alpha;
			atl.alpha->setName(alpha);
			atl.alpha->init(0, w, h, tf);				
		}			

		_atlasses.push_back(atl);
	}

	void ResAtlas::loadAtlas(CreateResourceContext &context)
	{
		string path = *context.folder;

		int w = context.node.attribute("width").as_int(2048);
		int h = context.node.attribute("height").as_int(2048);
		const char *format = context.node.attribute("format").as_string("8888");

		atlas_data ad;


		TextureFormat tf = string2TextureFormat(format);

		pugi::xml_node child_node = context.node.first_child();
		
		pugi::xml_node meta_image = context.meta.child("image");
		
		bool compressed = false;

		while(meta_image)
		{
			const char *file = meta_image.attribute("file").value();

			int w = meta_image.attribute("w").as_int();
			int h = meta_image.attribute("h").as_int();

			const char *file_format = meta_image.attribute("format").as_string(0);
			TextureFormat ffmt = TF_UNDEFINED;
			if (file_format)
			{
				ffmt = string2TextureFormat(file_format);
				compressed = isCompressedFormat(ffmt);
			}

			string alpha_file = meta_image.attribute("alpha").as_string("");
			if (!alpha_file.empty())
			{
				alpha_file = *context.prebuilt_folder + alpha_file;
			}

			addAtlas(tf, *context.prebuilt_folder + file, alpha_file, w, h);

			meta_image = meta_image.next_sibling("image");
		}

		pugi::xml_node meta_frames = context.meta.child("frames");

		float scaleFactor = context.scale_factor;


		vector<ResAnim*> anims;


		child_node = context.node.first_child();
		while (!child_node.empty())
		{
			const char *name = child_node.name();
			if (!strcmp(name, "set"))
			{
				pugi::xml_attribute attr = child_node.first_attribute();
				while(attr)
				{
					if (!strcmp(attr.name(), "path"))
					{
						path = getPath(path.c_str(), attr.value());
						path += "/";
					}
					if (!strcmp(attr.name(), "scale_factor"))
					{
						scaleFactor = attr.as_float(scaleFactor);
					}
					attr = attr.next_attribute();
				}				
			}
			else
			if (!strcmp(name, "image"))
			{
				string id = child_node.attribute("id").value();
				string file = child_node.attribute("file").value();

				if (file.empty())
				{
					ResAnim *ra = new ResAnim(this);
					ra->init(0, 0, 0, scaleFactor);
					init_resAnim(ra, file, child_node);
					context.resources->add(ra);
					child_node = child_node.next_sibling();
					continue;
				}

				if (context.meta)
				{
					OX_ASSERT(meta_frames && "Did you recreate atlasses?");
				}

								
				int virtual_frame_width = 0;
				
				MemoryTexture mt;
				ImageData im;

				int columns = 0;
				int rows = 0;
				int frame_width = 0;
				int frame_height = 0;
				float frame_scale = 1.0f;
				bool loaded = false;

				if (meta_frames  || context.meta)
				{
					const char *frame_size = meta_frames.attribute("fs").value();

					//int w = 0;
					//int h = 0;

					sscanf(frame_size, "%d,%d,%d,%d,%f", &columns, &rows, 
						&frame_width, &frame_height, 
						&frame_scale);
					loaded = true;
					//frame_scale /= scaleFactor;//todo! fix

					//im.w = w;
					//im.h = h;
				}
				else
				{
					file::buffer bf;
					file::read((path + file).c_str(), bf);

					mt.init(bf, Renderer::getPremultipliedAlphaRender(), tf);
					im = mt.lock();
					if (im.w)
					{
						rows = child_node.attribute("rows").as_int();
						frame_width = child_node.attribute("frame_width").as_int();
						columns = child_node.attribute("cols").as_int();
						frame_height = child_node.attribute("frame_height").as_int();

						if (!rows)
							rows = 1;

						if (!columns)
							columns = 1;

						if (frame_width)
							columns = im.w / frame_width;				
						else
							frame_width = im.w / columns;


						if (frame_height)				
							rows = im.h / frame_height;
						else
							frame_height = im.h / rows;				

					}
				}

				if (columns)
				{
					animationFrames frames;
					int frames_count = rows * columns;
					frames.reserve(frames_count);
					
					ResAnim *ra = new ResAnim(this);

					if (context.meta)
					{
						OX_ASSERT(meta_frames);

						/*
						if (string(meta_frames.attribute("debug_image").as_string()) == "backleft.png")
						{

						}
						*/

						char *frames_data = (char*)meta_frames.first_child().value();

						const char *begin = frames_data;
						int i = 0;
						while(*frames_data)
						{
							if (*frames_data == ';')
							{
								*frames_data = 0;
								int id = 0;
								int x = 0;
								int y = 0;

								int bbox_x = 0;
								int bbox_y = 0;
								int bbox_w = 0;
								int bbox_h = 0;

								sscanf(begin, "%d,%d,%d,%d,%d,%d,%d", &id, &x, &y, &bbox_x, &bbox_y, &bbox_w, &bbox_h);

								begin = frames_data + 1;

								spNativeTexture &texture = _atlasses[id].base;
								spNativeTexture &alpha = _atlasses[id].alpha;

								float iw = 1.0f / texture->getWidth();
								float ih = 1.0f / texture->getHeight();

								RectF srcRect(x * iw, y * ih, bbox_w * iw, bbox_h * ih);

								float fs = 1.0f / frame_scale;
								RectF destRect(
									Vector2((float)bbox_x, (float)bbox_y) * fs,
									Vector2((float)bbox_w, (float)bbox_h) * fs
									);

								AnimationFrame frame;
								Diffuse df;
								df.base = texture;
								df.alpha = alpha;
								//compressed data could not be premultiplied 
								if (Renderer::getPremultipliedAlphaRender())
									df.premultiplied = !compressed;
								else
									df.premultiplied = true;//render should think that it is already premultiplied and don't worry about alpha

								frame.init(ra, df,
									srcRect, destRect, 
									Vector2((float)frame_width, (float)frame_height) * fs);

								frames.push_back(frame);
								if((int)frames.size() >= frames_count)
									break;
							}

							++frames_data;
						}
					}
					else
					{
						anims.push_back(ra);

						for (int y = 0; y < rows; ++y)
						{
							for (int x = 0; x < columns; ++x)
							{
								Rect src;
								src.pos = Point(x * frame_width, y * frame_height);
								src.size = Point(frame_width, frame_height);

								ImageData srcImage = im.getRect(src);

								Rect dest(0,0,0,0);

								if (!ad.texture)
								{
									string atlas_id = getName();
									next_atlas(w, h, tf, ad, atlas_id.c_str());
								}

								bool s = ad.atlas.add(&ad.mt, srcImage, dest);
								if (s == false)
								{
									apply_atlas(ad);
									next_atlas(w, h, tf, ad, path.c_str());
									s = ad.atlas.add(&ad.mt, srcImage, dest);
									OX_ASSERT(s);
								}

								/*
								float iw = 1.0f / ad.mt.getWidth();
								float ih = 1.0f / ad.mt.getHeight();
								*/

								float iw = 1.0f;
								float ih = 1.0f;
								
								RectF srcRect(dest.pos.x * iw, dest.pos.y * ih, dest.size.x * iw, dest.size.y * ih);

								Vector2 sizeScaled = Vector2((float)dest.size.x, (float)dest.size.y) * scaleFactor;
								RectF destRect(Vector2(0, 0), sizeScaled);

								AnimationFrame frame;
								Diffuse df;
								df.base = ad.texture;
								df.premultiplied = true;//!Renderer::getPremultipliedAlphaRender();
								frame.init(ra, df, srcRect, destRect, Vector2((float)frame_width, (float)frame_height) * scaleFactor);
								frames.push_back(frame);
							}
						}
					}	


					
					init_resAnim(ra, file, child_node);
					
					ra->init(frames, columns, scaleFactor);					
					context.resources->add(ra);
				}


				if (context.meta)
				{
					OX_ASSERT(meta_frames);
					meta_frames = meta_frames.next_sibling();
				}

			}

			child_node = child_node.next_sibling();
		}

		apply_atlas(ad);

		for (vector<ResAnim*>::iterator i = anims.begin(); i != anims.end(); ++i)
		{
			ResAnim *rs = *i;
			int num = rs->getTotalFrames();

			for (int n = 0; n < num; ++n)
			{
				AnimationFrame &frame = const_cast<AnimationFrame&>(rs->getFrame(n));
				
				float iw = 1.0f / frame.getDiffuse().base->getWidth();
				float ih = 1.0f / frame.getDiffuse().base->getHeight();

				RectF rect = frame.getSrcRect();
				rect.pos.x *= iw;
				rect.pos.y *= ih;
				rect.size.x *= iw;
				rect.size.y *= ih;
				frame.setSrcRect(rect);
			}
		}
	}

	Resource *ResAtlas::create(CreateResourceContext &context)
	{
		ResAtlas *ra = new ResAtlas();
		ra->setName(Resource::extractID(context.node, "", string("!atlas:") + *context.xml_name));
		ra->loadAtlas(context);
		setNode(ra, context.node);
		context.meta = context.meta.next_sibling();
		return ra;
	}

	ResAtlas::ResAtlas()
	{

	}

	ResAtlas::~ResAtlas()
	{
		for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
		{
			atlas &atl = *i;
			if (atl.base)
				atl.base->release();
			if (atl.alpha)
				atl.alpha->release();
		}
	}

	void load_texture(const string &file, spNativeTexture nt, LoadResourcesContext *load_context)
	{
		ImageData im;
		spMemoryTexture mt = new MemoryTexture;

		LOGD("loading atlas: %s", file.c_str());
		file::buffer bf;
		file::read(file.c_str(), bf);
		LOGD("atlas file loaded: %s", file.c_str());
		mt->init(bf, Renderer::getPremultipliedAlphaRender(), nt->getFormat());
		//mt->init(2048, 2048, TF_R8G8B8A8);
		im = mt->lock();
		LOGD("atlas size: %d %d", im.w, im.h);

		//Object::dumpCreatedObjects();
		load_context->createTexture(mt, nt);
	}

	void ResAtlas::_restore(Restorable *r, void *)
	{
		NativeTexture *texture = (NativeTexture *)r->_getRestorableObject();

		for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
		{
			atlas &atl = *i;
			if(atl.base.get() == texture)
			{
				load_texture(atl.base_path, atl.base, &RestoreResourcesContext::instance);
				atl.base->reg(CLOSURE(this, &ResAtlas::_restore), 0);
				break;
			}

			if(atl.alpha.get() == texture)
			{
				load_texture(atl.alpha_path, atl.alpha, &RestoreResourcesContext::instance);
				atl.alpha->reg(CLOSURE(this, &ResAtlas::_restore), 0);
				break;
			}
		}		
	}

	void ResAtlas::_load(LoadResourcesContext *load_context)
	{
		for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
		{
			atlas &atl = *i;
			if (!load_context->isNeedProceed(atl.base))
				continue;
			
			load_texture(atl.base_path, atl.base, load_context);
			atl.base->reg(CLOSURE(this, &ResAtlas::_restore), 0);

			if (atl.alpha)
			{
				load_texture(atl.alpha_path, atl.alpha, load_context);
				atl.alpha->reg(CLOSURE(this, &ResAtlas::_restore), 0);
			}
		}
	}

	void ResAtlas::_unload()
	{
		for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
		{
			atlas &atl = *i;
			if (atl.base)					
				atl.base->release();
			
			if (atl.alpha)
				atl.alpha->release();
		}
	}
}