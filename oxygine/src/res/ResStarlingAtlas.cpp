#include "ResStarlingAtlas.h"
#include "CreateResourceContext.h"
#include "core/file.h"
#include "ResAnim.h"
#include "Resources.h"
#include "core/VideoDriver.h"
#include "core/NativeTexture.h"
#include "MemoryTexture.h"

namespace oxygine
{
	Resource *ResStarlingAtlas::create(CreateResourceContext &context)
	{
		ResStarlingAtlas *ra = new ResStarlingAtlas();
		ra->setName(string("!atlas:") + *context.xml_name);
		ra->loadAtlas(context);
		setNode(ra, context.node);
		context.meta = context.meta.next_sibling();
		return ra;
	}

	void ResStarlingAtlas::loadAtlas(CreateResourceContext &context)
	{
		const char *xml_name = context.node.attribute("file").value();

		string xml_path = *context.folder + xml_name;		


		file::buffer fb;
		file::read(xml_path.c_str(), fb);

		pugi::xml_document doc;
		doc.load_buffer_inplace(&fb.data[0], fb.data.size());

		pugi::xml_node starling_xml = doc.first_child();
		_imagePath = *context.folder + starling_xml.attribute("imagePath").value();

		_texture = IVideoDriver::instance->createTexture();

		if (context.meta)
		{
			int textureWidth = context.meta.attribute("tw").as_int();
			int textureHeight = context.meta.attribute("th").as_int();
			_texture->init(0, textureWidth, textureHeight, TF_R8G8B8A8);
		}
		else
		{
			spMemoryTexture mt = new MemoryTexture;

			ImageData im;
			file::buffer bf;
			file::read(_imagePath.c_str(), bf);

			mt->init(bf, true, _texture->getFormat());
			im = mt->lock();
			_texture->init(mt->lock(), false);
		}
		

		float iw = 1.0f / _texture->getWidth();
		float ih = 1.0f / _texture->getHeight();

		ResAnim *resAnim = 0;
		pugi::xml_node sub = starling_xml.first_child();

		animationFrames frames;

		while (sub)
		{
			const char *name = sub.attribute("name").value();
			int index_pos = strlen(name) - 4;
			const char *index = name + index_pos;
			char id[255];
			strcpy(id, name);
			id[index_pos] = 0;

			int frame_index = atoi(index);

			//todo optimize attributes 
			int x = sub.attribute("x").as_int();
			int y = sub.attribute("y").as_int();
			int width = sub.attribute("width").as_int();
			int height = sub.attribute("height").as_int();

			int frameX = sub.attribute("frameX").as_int();
			int frameY = sub.attribute("frameY").as_int();
			int frameWidth = sub.attribute("frameWidth").as_int(width);
			int frameHeight = sub.attribute("frameHeight").as_int(height);

			if (!resAnim || resAnim->getName() != id)
			{
				if (resAnim)
				{
					resAnim->init(frames, frames.size());
					context.resources->add(resAnim);
					frames.clear();
				}

				resAnim = new ResAnim(this);
				setNode(resAnim, context.node);
				resAnim->setName(id);
			}

			AnimationFrame frame;
			RectF srcRect(x * iw, y * ih, width * iw, height * ih);
			RectF destF = Rect(-frameX, -frameY, width, height).cast<RectF>();

			Diffuse df;
			df.base = _texture;
			frame.init(resAnim, df, srcRect, destF, Vector2((float)frameWidth, (float)frameHeight));
			frames.push_back(frame);

			sub = sub.next_sibling();
		}

		if (resAnim)
		{
			resAnim->init(frames, frames.size());
			context.resources->add(resAnim);
		}


	}

	ResStarlingAtlas::ResStarlingAtlas()
	{

	}

	ResStarlingAtlas::~ResStarlingAtlas()
	{

	}

	void ResStarlingAtlas::_load(LoadResourcesContext *load_context)
	{
		if (_texture->getHandle())
			return;

		spMemoryTexture mt = new MemoryTexture;

		ImageData im;

		file::buffer bf;
		file::read(_imagePath.c_str(), bf);
			
		mt->init(bf, true, _texture->getFormat());
		im = mt->lock();
			
		if (load_context)
			load_context->createTexture(mt, _texture);
		else
			_texture->init(mt->lock(), false);
	}

	void ResStarlingAtlas::_unload()
	{
		_texture->release();
	}
}