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
#include <stdint.h>

extern "C"
{
#include "utils/cdecode.h"
}

#ifdef __S3E__
#include "IwImage.h"
#endif

//#define  LOGD(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)

namespace oxygine
{
    int defaultAtlasWidth = 2048;
    int defaultAtlasHeight = 2048;

    struct atlas_data
    {
        spNativeTexture texture;
        MemoryTexture mt;
        Atlas2 atlas;
    };


    void apply_atlas(atlas_data& ad)
    {
        if (!ad.texture)
            return;

        MemoryTexture mt;
        const Rect& bounds = ad.atlas.getBounds();
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

    void next_atlas(int w, int h, TextureFormat tf, atlas_data& ad, const char* name = 0)
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


    static load_texture_hook _hook = 0;
    void set_load_texture_hook(load_texture_hook hook)
    {
        _hook = hook;
    }

    void load_texture_internal(const std::string& file, spNativeTexture nt, bool linearFilter, LoadResourcesContext* load_context)
    {
        ImageData im;
        spMemoryTexture mt = new MemoryTexture;

        LOGD("loading atlas: %s", file.c_str());
        file::buffer bf;
        file::read(file.c_str(), bf);
        LOGD("atlas file loaded: %s", file.c_str());
        mt->init(bf, true, nt->getFormat());
        im = mt->lock();
        LOGD("atlas size: %d %d", im.w, im.h);

        CreateTextureTask opt;
        opt.src = mt;
        opt.dest = nt;
        opt.linearFilter = linearFilter;
        load_context->createTexture(opt);
    }

    void load_texture(const std::string& file, spNativeTexture nt, bool linearFilter, LoadResourcesContext* load_context)
    {
        if (_hook)
        {
            _hook(file, nt, load_context);
            return;
        }

        load_texture_internal(file, nt, linearFilter, load_context);
    }


    void ResAtlas::init_resAnim(ResAnim* rs, const std::string& file, pugi::xml_node node)
    {
        rs->setName(_Resource::extractID(node, file, ""));
        setNode(rs, node);
    }

    void ResAtlas::addAtlas(TextureFormat tf, const std::string& base, const std::string& alpha, int w, int h)
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

    Resource* ResAtlas::create(CreateResourceContext& context)
    {
        context.walker.checkSetAttributes();
        ResAtlas* ra = 0;
        if (context.walker.getMeta())
        {
            ResAtlasPrebuilt* rs = new ResAtlasPrebuilt(context);

            ra = rs;
        }
        else
        {
            ResAtlasGeneric* rs = new ResAtlasGeneric();
            rs->loadAtlas(context);

            ra = rs;
        }

        context.resources->add(ra);

        ra->setName(_Resource::extractID(context.walker.getNode(), "", std::string("!atlas:") + *context.xml_name));
        setNode(ra, context.walker.getNode());

        return ra;
    }

    ResAtlas::ResAtlas(): _linearFilter(true), _clamp2edge(true)
    {

    }

    ResAtlas::~ResAtlas()
    {
        for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
        {
            atlas& atl = *i;
            if (atl.base)
                atl.base->release();
            if (atl.alpha)
                atl.alpha->release();
        }
    }


    void ResAtlas::loadBase(pugi::xml_node node)
    {
        _linearFilter = node.attribute("linearFilter").as_bool(true);
        _clamp2edge = node.attribute("clamp2edge").as_bool(true);
    }

    void ResAtlas::_restore(Restorable* r, void*)
    {
        NativeTexture* texture = (NativeTexture*)r->_getRestorableObject();

        for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
        {
            atlas& atl = *i;
            if (atl.base.get() == texture)
            {
                load_texture(atl.base_path, atl.base, _linearFilter , &RestoreResourcesContext::instance);
                atl.base->reg(CLOSURE(this, &ResAtlas::_restore), 0);
                break;
            }

            if (atl.alpha.get() == texture)
            {
                load_texture(atl.alpha_path, atl.alpha, _linearFilter, &RestoreResourcesContext::instance);
                atl.alpha->reg(CLOSURE(this, &ResAtlas::_restore), 0);
                break;
            }
        }
    }

    void ResAtlas::_load(LoadResourcesContext* load_context)
    {
        for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
        {
            atlas& atl = *i;
            if (!load_context->isNeedProceed(atl.base))
                continue;

            load_texture(atl.base_path, atl.base, _linearFilter, load_context);
            atl.base->reg(CLOSURE(this, &ResAtlas::_restore), 0);

            if (atl.alpha)
            {
                load_texture(atl.alpha_path, atl.alpha, _linearFilter, load_context);
                atl.alpha->reg(CLOSURE(this, &ResAtlas::_restore), 0);
            }
        }
    }

    void ResAtlas::_unload()
    {
        for (atlasses::iterator i = _atlasses.begin(); i != _atlasses.end(); ++i)
        {
            atlas& atl = *i;
            if (atl.base)
                atl.base->release();

            if (atl.alpha)
                atl.alpha->release();
        }
    }

    ResAnim* ResAtlas::createEmpty(const XmlWalker& walker, CreateResourceContext& context)
    {
        ResAnim* ra = new ResAnim(this);
        ra->init(0, 0, 0, walker.getScaleFactor());
        init_resAnim(ra, "", walker.getNode());
        ra->setParent(this);
        context.resources->add(ra);

        return ra;
    }


    int roundUp(int numToRound, int multiple)
    {
        if (multiple == 0)
            return numToRound;

        int remainder = numToRound % multiple;
        if (remainder == 0)
            return numToRound;
        return numToRound + multiple - remainder;
    }


    int HIT_TEST_DOWNSCALE = 4;
    const int ALIGN = sizeof(int32_t);
    const int BITS = ALIGN * 8;

    void makeAlpha(const ImageData& srcImage, Rect& bounds, std::vector<unsigned char>& alpha, HitTestData& adata, bool hittest)
    {
        int w = srcImage.w;
        int h = srcImage.h;

        size_t pos = alpha.size();
        adata.data = reinterpret_cast<unsigned char*>(pos);
        adata.w = roundUp(w, HIT_TEST_DOWNSCALE) / HIT_TEST_DOWNSCALE;
        adata.h = roundUp(h, HIT_TEST_DOWNSCALE) / HIT_TEST_DOWNSCALE;


        int lineInts = roundUp(adata.w, BITS) / BITS;

        int destPitch = lineInts * ALIGN;

        int size = adata.h * destPitch;

        alpha.resize(pos + size);


        const unsigned char* srcData = srcImage.data;
        int srcStep = srcImage.bytespp;
        int srcPitch = srcImage.pitch;

        unsigned char* destData = &alpha[pos];

        adata.pitch = destPitch;

        const unsigned char* srcRow = srcData;
        unsigned char* destRow = destData;


        int minX = w;
        int minY = h;
        int maxX = 0;
        int maxY = 0;

        bool hasAlpha = false;

        for (int y = 0; y != h; y += 1)
        {
            const unsigned char* srcLine = srcRow;
            int32_t* destLine = reinterpret_cast<int32_t*>(destRow);

            bool lineWithAlpha = false;


            for (int x = 0; x != w; x += 1)
            {
                PixelR8G8B8A8 pd;
                Pixel p;
                pd.getPixel(srcLine, p);


                if (p.a > 10)
                {
                    hasAlpha = true;

                    int dx = x / HIT_TEST_DOWNSCALE;
                    int n = dx / BITS;
                    int b = dx % BITS;

                    destLine[n] |= 1 << b;

                    lineWithAlpha = true;
                    if (x > maxX)
                        maxX = x;
                    else if (x < minX)
                        minX = x;
                }
                srcLine += srcStep;
            }

            if (lineWithAlpha)
            {
                if (minY == h)
                    minY = y;
                maxY = y;
            }

            if (y % HIT_TEST_DOWNSCALE == HIT_TEST_DOWNSCALE - 1)
            {
                //reset line
                destRow += destPitch;
            }

            srcRow += srcPitch;
        }

        //if image is transparent
        if (minX == w && maxX == 0)
        {
            minX = 0;
            maxX = 0;
        }

        if (minY == h && maxY == 0)
        {
            minY = 0;
            maxY = 0;
        }

        bounds = Rect(minX, minY, maxX - minX + 1, maxY - minY + 1);

        if (!hasAlpha || !hittest)
        {
            alpha.resize(pos);
            adata = HitTestData();
        }
    }



    void ResAtlasGeneric::loadAtlas(CreateResourceContext& context)
    {
        pugi::xml_node node = context.walker.getNode();

        int w = node.attribute("width").as_int(defaultAtlasWidth);
        int h = node.attribute("height").as_int(defaultAtlasHeight);
        const char* format = node.attribute("format").as_string("8888");

        loadBase(node);

        atlas_data ad;


        TextureFormat tf = string2TextureFormat(format);


        bool compressed = false;


        std::vector<ResAnim*> anims;


        while (true)
        {
            XmlWalker walker = context.walker.next();
            if (walker.empty())
                break;

            pugi::xml_node child_node = walker.getNode();

            const char* name = child_node.name();
            if (strcmp(name, "image"))
                continue;


            std::string id = child_node.attribute("id").value();
            std::string file = child_node.attribute("file").value();

            if (file.empty())
            {
                createEmpty(walker, context);
                continue;
            }



            MemoryTexture mt;
            ImageData im;

            int columns = 0;
            int rows = 0;
            int frame_width = 0;
            int frame_height = 0;
            float frame_scale = 1.0f;
            bool loaded = false;


            file::buffer bf;
            file::read(walker.getPath("file").c_str(), bf);

            mt.init(bf, true, tf);
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


            if (columns)
            {
                animationFrames frames;
                int frames_count = rows * columns;
                frames.reserve(frames_count);

                ResAnim* ra = new ResAnim(this);


                anims.push_back(ra);

                for (int y = 0; y < rows; ++y)
                {
                    for (int x = 0; x < columns; ++x)
                    {
                        Rect frameRect;
                        frameRect.pos = Point(x * frame_width, y * frame_height);
                        frameRect.size = Point(frame_width, frame_height);

                        ImageData srcImage_ = im.getRect(frameRect);


                        HitTestData adata;
                        ImageData src;
                        Rect bounds;
                        makeAlpha(srcImage_, bounds, _hitTestBuffer, adata, walker.getAlphaHitTest());
                        src = srcImage_.getRect(bounds);


                        //mt.init(src.w + 4, src.h + 4, src.format);
                        //mt.fill_zero();



                        Rect dest(0, 0, 0, 0);

                        if (!ad.texture)
                        {
                            std::string atlas_id = getName();
                            next_atlas(w, h, tf, ad, atlas_id.c_str());
                        }

                        bool s = ad.atlas.add(&ad.mt, src, dest);
                        if (s == false)
                        {
                            apply_atlas(ad);
                            next_atlas(w, h, tf, ad, walker.getCurrentFolder().c_str());
                            s = ad.atlas.add(&ad.mt, src, dest);
                            OX_ASSERT(s);
                        }

                        //duplicate image edges
                        MemoryTexture& mt = ad.mt;
                        operations::copy(src.getRect(Rect(0, 0, src.w, 1)),         mt.lock(&Rect(dest.pos.x, dest.pos.y - 1,    src.w, 1)));
                        operations::copy(src.getRect(Rect(0, src.h - 1, src.w, 1)), mt.lock(&Rect(dest.pos.x, dest.pos.y + src.h, src.w, 1)));
                        operations::copy(src.getRect(Rect(0, 0, 1, src.h)),         mt.lock(&Rect(dest.pos.x - 1, dest.pos.y,    1, src.h)));
                        operations::copy(src.getRect(Rect(src.w - 1, 0, 1, src.h)), mt.lock(&Rect(dest.pos.x + src.w, dest.pos.y, 1, src.h)));


                        float iw = 1.0f;
                        float ih = 1.0f;

                        RectF srcRect(dest.pos.x * iw, dest.pos.y * ih, dest.size.x * iw, dest.size.y * ih);

                        Vector2 sizeScaled = Vector2((float)dest.size.x, (float)dest.size.y) * walker.getScaleFactor();
                        RectF destRect(bounds.pos.cast<Vector2>(), sizeScaled);

                        AnimationFrame frame;
                        Diffuse df;
                        df.base = ad.texture;
                        df.premultiplied = true;//!Renderer::getPremultipliedAlphaRender();

                        Vector2 fsize = Vector2((float)frame_width, (float)frame_height) * walker.getScaleFactor();
                        frame.init2(ra, x, y, df,
                                    srcRect, destRect, fsize);

                        frame.setHitTestData(adata);

                        frames.push_back(frame);
                    }
                }

                init_resAnim(ra, file, child_node);

                ra->init(frames, columns, walker.getScaleFactor(), 1.0f / walker.getScaleFactor());
                ra->setParent(this);
                context.resources->add(ra);
            }

        }

        apply_atlas(ad);

        for (std::vector<ResAnim*>::iterator i = anims.begin(); i != anims.end(); ++i)
        {
            ResAnim* rs = *i;
            int num = rs->getTotalFrames();

            for (int n = 0; n < num; ++n)
            {
                AnimationFrame& frame = const_cast<AnimationFrame&>(rs->getFrame(n));

                float iw = 1.0f / frame.getDiffuse().base->getWidth();
                float ih = 1.0f / frame.getDiffuse().base->getHeight();

                RectF rect = frame.getSrcRect();
                rect.pos.x *= iw;
                rect.pos.y *= ih;
                rect.size.x *= iw;
                rect.size.y *= ih;
                frame.setSrcRect(rect);

                HitTestData ad = frame.getHitTestData();
                if (ad.pitch)
                {
                    ad.data = &_hitTestBuffer[reinterpret_cast<size_t>(ad.data)];
                    frame.setHitTestData(ad);
                }
            }
        }
    }


    ResAtlasPrebuilt::ResAtlasPrebuilt(CreateResourceContext& context)
    {
        pugi::xml_node node = context.walker.getNode();
        pugi::xml_node meta = context.walker.getMeta();

        const char* format = node.attribute("format").as_string("8888");

        loadBase(node);


        TextureFormat tf = string2TextureFormat(format);

        pugi::xml_node meta_image = meta.child("atlas");

        pugi::xml_node meta_alpha = meta.child("ht");
        if (meta_alpha)
        {
            const char* data = meta_alpha.text().as_string();
            int len = meta_alpha.attribute("len").as_int();
            base64_decodestate state;
            base64_init_decodestate(&state);
            _hitTestBuffer.resize(len * 3 / 4);
            if (len)
                base64_decode_block(data, len, (char*)&_hitTestBuffer.front(), &state);
        }

        bool compressed = false;

        while (meta_image)
        {
            const char* file = meta_image.attribute("file").value();

            int w = meta_image.attribute("w").as_int();
            int h = meta_image.attribute("h").as_int();

            const char* file_format = meta_image.attribute("format").as_string(0);
            TextureFormat ffmt = TF_UNDEFINED;
            if (file_format)
            {
                ffmt = string2TextureFormat(file_format);
                compressed = isCompressedFormat(ffmt);
            }

            std::string alpha_file = meta_image.attribute("alpha").as_string("");
            if (!alpha_file.empty())
            {
                alpha_file = *context.prebuilt_folder + alpha_file;
            }

            addAtlas(tf, *context.prebuilt_folder + file, alpha_file, w, h);

            meta_image = meta_image.next_sibling("atlas");
            context.walker.nextMeta();
        }

        while (true)
        {
            XmlWalker walker = context.walker.next();
            if (walker.empty())
                break;

            pugi::xml_node child_node = walker.getNode();

            const char* name = child_node.name();
            if (strcmp(name, "image"))
                continue;

            pugi::xml_node meta_frames = walker.getMeta();

            std::string id = child_node.attribute("id").value();
            std::string file = child_node.attribute("file").value();

            if (file.empty())
            {
                createEmpty(walker, context);
                continue;
            }


            OX_ASSERT(meta_frames && "Did you recreate atlasses?");

            int columns = 0;
            int rows = 0;
            int frame_width = 0;
            int frame_height = 0;
            float frame_scale = 1.0f;


            const char* frame_size = meta_frames.attribute("fs").value();

            sscanf(frame_size, "%d,%d,%d,%d,%f", &columns, &rows,
                   &frame_width, &frame_height,
                   &frame_scale);

            HitTestData adata;
            const char* alpha = meta_frames.attribute("ht").as_string(0);
            if (alpha)
            {
                int ad_w, ad_h, ad_pos, ad_size;
                sscanf(alpha, "%d,%d,%d,%d", &ad_pos, &ad_size, &ad_w, &ad_h);
                adata.w = ad_w;
                adata.h = ad_h;
                adata.pitch = ad_size / ad_h;
                adata.data = &_hitTestBuffer[ad_pos];
            }



            if (columns)
            {
                animationFrames frames;
                int frames_count = rows * columns;
                frames.reserve(frames_count);

                ResAnim* ra = new ResAnim(this);


                OX_ASSERT(meta_frames);


                char* frames_data = (char*)meta_frames.first_child().value();


                const char* begin = frames_data;
                while (*frames_data)
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

                        spNativeTexture& texture = _atlasses[id].base;
                        spNativeTexture& alpha = _atlasses[id].alpha;

                        float iw = 1.0f / texture->getWidth();
                        float ih = 1.0f / texture->getHeight();

                        RectF srcRect(x * iw, y * ih, bbox_w * iw, bbox_h * ih);

                        float fs = frame_scale;
                        RectF destRect(
                            Vector2((float)bbox_x, (float)bbox_y) * fs,
                            Vector2((float)bbox_w, (float)bbox_h) * fs
                        );

                        AnimationFrame frame;
                        Diffuse df;
                        df.base = texture;
                        df.alpha = alpha;

                        //compressed data could not be premultiplied
                        df.premultiplied = !compressed;

                        size_t n = frames.size();
                        int column = n % columns;
                        int row = (int)(n / columns);

                        frame.init2(ra, column, row, df,
                                    srcRect, destRect,
                                    Vector2((float)frame_width, (float)frame_height));
                        if (adata.pitch)
                        {
                            frame.setHitTestData(adata);
                            adata.data += adata.h * adata.pitch;
                        }

                        frames.push_back(frame);
                        if ((int)frames.size() >= frames_count)
                            break;
                    }

                    ++frames_data;
                }

                init_resAnim(ra, file, child_node);

                ra->init(frames, columns, walker.getScaleFactor(), 1.0f / frame_scale);
                ra->setParent(this);
                context.resources->add(ra);
            }
        }
    }
}