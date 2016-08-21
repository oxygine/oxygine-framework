#include "ResStarlingAtlas.h"
#include "CreateResourceContext.h"
#include "core/file.h"
#include "ResAnim.h"
#include "Resources.h"
#include "core/VideoDriver.h"
#include "core/NativeTexture.h"
#include "Image.h"
#include "CreateResourceContext.h"
#include "utils/stringUtils.h"

namespace oxygine
{
    Resource* ResStarlingAtlas::create(CreateResourceContext& context)
    {
        ResStarlingAtlas* ra = new ResStarlingAtlas();
        ra->setName(std::string("!atlas:") + *context.xml_name);
        ra->loadAtlas(context);
        setNode(ra, context.walker.getNode());
        return ra;
    }

    void ResStarlingAtlas::loadAtlas(CreateResourceContext& context)
    {
        std::string xml_path = context.walker.getPath("file");

        file::buffer fb;
        file::read(xml_path, fb);

        pugi::xml_document doc;
        doc.load_buffer_inplace(&fb.data[0], fb.data.size());

        pugi::xml_node starling_xml = doc.first_child();

        pugi::xml_node root = doc.root().first_child();
        const std::string& img = root.attribute("imagePath").as_string();

        char head[255];
        char tail[255];
        path::split(xml_path.c_str(), head, tail);
        _imagePath = std::string(head) + img;

        _texture = IVideoDriver::instance->createTexture();

        pugi::xml_node meta = context.walker.getMeta();

        if (!meta.empty())
        {
            int textureWidth = meta.attribute("tw").as_int();
            int textureHeight = meta.attribute("th").as_int();
            _texture->init(0, textureWidth, textureHeight, TF_R8G8B8A8);
        }
        else
        {

            unsigned char buff[64];
            unsigned int size = 0;
            {
                file::autoClose ac(file::open(_imagePath, "rb"));
                size = file::read(ac.getHandle(), buff, sizeof(buff));
            }

            int width = 0;
            int height = 0;
            ImageType type;

            if (getImageInfo(buff, size, img.c_str(), type, width, height))
            {
                _texture->init(0, width, height, TF_R8G8B8A8);
            }
            else
            {
                spImage mt = new Image;

                ImageData im;
                file::buffer bf;
                file::read(_imagePath, bf);

                mt->init(bf, true, _texture->getFormat());
                im = mt->lock();
                _texture->init(mt->lock(), false);
            }
        }


        float iw = 1.0f / _texture->getWidth();
        float ih = 1.0f / _texture->getHeight();

        ResAnim* resAnim = 0;
        pugi::xml_node sub = starling_xml.first_child();

        animationFrames frames;

        while (sub)
        {
            const char* name = sub.attribute("name").value();

            char id[255];
            strcpy(id, name);

            //int frame_index = atoi(index);

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
                    resAnim->init(frames, (int)frames.size());
                    resAnim->setParent(this);
                    context.resources->add(resAnim);
                    frames.clear();
                }

                resAnim = new ResAnim(this);
                setNode(resAnim, context.walker.getNode());
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
            resAnim->init(frames, (int)frames.size());
            resAnim->setParent(this);
            context.resources->add(resAnim);
        }
    }

    ResStarlingAtlas::ResStarlingAtlas()
    {

    }

    ResStarlingAtlas::~ResStarlingAtlas()
    {

    }

    void load_texture(const std::string& file, spNativeTexture nt, bool linearFilter, bool clamp2edge, LoadResourcesContext* load_context);

    void ResStarlingAtlas::_restore(Restorable* r, void*)
    {
        load_texture(_imagePath, _texture, true, true, &RestoreResourcesContext::instance);
        _texture->reg(CLOSURE(this, &ResStarlingAtlas::_restore), 0);
    }

    void ResStarlingAtlas::_load(LoadResourcesContext* load_context)
    {
        if (!load_context->isNeedProceed(_texture))
            return;

        load_texture(_imagePath, _texture, true, true, load_context);
        _texture->reg(CLOSURE(this, &ResStarlingAtlas::_restore), 0);
    }

    void ResStarlingAtlas::_unload()
    {
        _texture->release();
    }
}