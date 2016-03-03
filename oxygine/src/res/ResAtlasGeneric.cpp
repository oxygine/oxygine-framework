#include "ResAtlasGeneric.h"
#include "core/ImageDataOperations.h"
#include "MemoryTexture.h"
#include "utils/AtlasTool.h"
#include "core/VideoDriver.h"
#include "Resources.h"

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


                if (p.a > 5)
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




    void ResAtlasGeneric::applyAtlas(atlas_data& ad, bool linear, bool clamp2edge)
    {
        if (!ad.texture)
            return;

        spMemoryTexture mt = new MemoryTexture;
        Rect bounds = ad.atlas.getBounds();

        int w = nextPOT(bounds.getRight());
        int h = nextPOT(bounds.getBottom());

        mt->init(ad.mt.lock().getRect(Rect(0, 0, w, h)));
#if 0
        static int n = 0;
        n++;
        char name[255];
        safe_sprintf(name, "test%d.tga", n);
        saveImage(image_data, name);
#endif

        CreateTextureTask task;
        task.linearFilter = linear;
        task.clamp2edge = clamp2edge;
        task.src = mt;
        task.dest = ad.texture;
        LoadResourcesContext::get()->createTexture(task);
    }

    void ResAtlasGeneric::nextAtlas(int w, int h, TextureFormat tf, atlas_data& ad, const char* name)
    {
        ad.mt.init(w, h, tf);
        ad.mt.fill_zero();

        ad.atlas.clean();
        ad.atlas.init(w, h);


        if (_atlasses.size() > _current)
            ad.texture = _atlasses[_current].base;
        else
        {
            ad.texture = IVideoDriver::instance->createTexture();

            atlas atl;
            atl.base = ad.texture;
            _atlasses.push_back(atl);
        }
        ad.texture->setName(name);

        _current++;
    }


    void ResAtlasGeneric::_unload()
    {
    }

    void ResAtlasGeneric::_load(LoadResourcesContext*)
    {
        //CreateResourceContext copy2 = _copy;
        //loadAtlas2(copy2);
    }

    void ResAtlasGeneric::loadAtlas(CreateResourceContext& context)
    {
        //_xmlFolder = context.walker.getXmlFolder();
        //_copy = context;
        //_copy.walker.setXmlFolder(&_xmlFolder);
        loadAtlas2(context);
    }

    void ResAtlasGeneric::loadAtlas2(CreateResourceContext& context)
    {
        _current = 0;
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


            bool trim = child_node.attribute("trim").as_bool(true);
            bool extend = child_node.attribute("extend").as_bool(true);

            Point offset = extend ? Point(2, 2) : Point(0, 0);

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
                        Rect bounds(0, 0, im.w, im.h);
                        if (trim)
                            makeAlpha(srcImage_, bounds, _hitTestBuffer, adata, walker.getAlphaHitTest());
                        src = srcImage_.getRect(bounds);

                        Rect dest(0, 0, 0, 0);

                        if (!ad.texture)
                        {
                            std::string atlas_id = getName();
                            nextAtlas(w, h, tf, ad, atlas_id.c_str());
                        }

                        bool s = ad.atlas.add(&ad.mt, src, dest, offset);
                        if (s == false)
                        {
                            applyAtlas(ad, _linearFilter, _clamp2edge);

                            nextAtlas(w, h, tf, ad, walker.getCurrentFolder().c_str());
                            s = ad.atlas.add(&ad.mt, src, dest, offset);
                            OX_ASSERT(s);
                        }

                        //extend = false;
                        if (extend)
                        {
                            //duplicate image edges
                            MemoryTexture& mt = ad.mt;
                            ImageData tmp;

                            if (bounds.getY() == 0)
                            {
                                tmp = mt.lock(Rect(dest.pos.x, dest.pos.y - 1, src.w, 1));
                                operations::copy(src.getRect(Rect(0, 0, src.w, 1)), tmp);
                            }

                            if (bounds.getHeight() == im.h)
                            {
                                tmp = mt.lock(Rect(dest.pos.x, dest.pos.y + src.h, src.w, 1));
                                operations::copy(src.getRect(Rect(0, src.h - 1, src.w, 1)), tmp);
                            }

                            if (bounds.getX() == 0)
                            {
                                tmp = mt.lock(Rect(dest.pos.x - 1, dest.pos.y, 1, src.h));
                                operations::copy(src.getRect(Rect(0, 0, 1, src.h)), tmp);
                            }

                            if (bounds.getWidth() == im.w)
                            {
                                tmp = mt.lock(Rect(dest.pos.x + src.w, dest.pos.y, 1, src.h));
                                operations::copy(src.getRect(Rect(src.w - 1, 0, 1, src.h)), tmp);
                            }
                        }


                        //operations::copy(src.getRect(Rect(0, 0, 1, 1)), mt.lock(&Rect(dest.pos.x - 1, dest.pos.y - 1, 1, 1)));
                        //operations::copy(src.getRect(Rect(src.w - 1, 0, 1, 1)), mt.lock(&Rect(dest.pos.x + src.w, dest.pos.y - 1, 1, 1)));

                        //operations::copy(src.getRect(Rect(0, src.h - 1, 1, 1)), mt.lock(&Rect(dest.pos.x - 1, dest.pos.y + src.h, 1, 1)));
                        //operations::copy(src.getRect(Rect(src.w - 1, src.h - 1, 1, 1)), mt.lock(&Rect(dest.pos.x + src.w, dest.pos.y + src.h, 1, 1)));


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
                context.resources->add(ra, context.options->_shortenIDS);
            }

        }

        applyAtlas(ad, _linearFilter, _clamp2edge);

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

}