#include "ResAtlas.h"
#include "ResAtlasGeneric.h"
#include "ResAtlasPrebuilt.h"
#include "Resources.h"
#include "../Image.h"
#include "../core/VideoDriver.h"

#ifdef __S3E__
#include "IwImage.h"
#endif

//#define  LOGD(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)

namespace oxygine
{
    static load_texture_hook _hook = 0;
    void set_load_texture_hook(load_texture_hook hook)
    {
        _hook = hook;
    }

    void load_texture_internal(const std::string& file, spNativeTexture nt, bool linearFilter, bool clamp2edge, LoadResourcesContext* load_context)
    {
        ImageData im;
        spImage mt = new Image;

        LOGD("loading atlas: %s", file.c_str());
        file::buffer bf;
        file::read(file, bf);
        LOGD("atlas file loaded: %s", file.c_str());
        mt->init(bf, true, nt->getFormat());
        im = mt->lock();
        LOGD("atlas size: %d %d", im.w, im.h);

        CreateTextureTask opt;
        opt.src = mt;
        opt.dest = nt;
        opt.linearFilter = linearFilter;
        opt.clamp2edge = clamp2edge;
        load_context->createTexture(opt);
    }

    void load_texture(const std::string& file, spNativeTexture nt, bool linearFilter, bool clamp2edge, LoadResourcesContext* load_context)
    {
        if (_hook)
        {
            _hook(file, nt, linearFilter, clamp2edge, load_context);
            return;
        }

        load_texture_internal(file, nt, linearFilter, clamp2edge, load_context);
    }


    void ResAtlas::init_resAnim(ResAnim* rs, const std::string& file, pugi::xml_node node)
    {
        rs->setName(Resource::extractID(node, file, ""));
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

        ra->setName(Resource::extractID(context.walker.getNode(), "", std::string("!atlas:") + *context.xml_name));
        context.resources->add(ra);
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
                load_texture(atl.base_path, atl.base, _linearFilter, _clamp2edge, &RestoreResourcesContext::instance);
                atl.base->reg(CLOSURE(this, &ResAtlas::_restore), 0);
                break;
            }

            if (atl.alpha.get() == texture)
            {
                load_texture(atl.alpha_path, atl.alpha, _linearFilter, _clamp2edge, &RestoreResourcesContext::instance);
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

            load_texture(atl.base_path, atl.base, _linearFilter, _clamp2edge, load_context);
            atl.base->reg(CLOSURE(this, &ResAtlas::_restore), 0);

            if (atl.alpha)
            {
                load_texture(atl.alpha_path, atl.alpha, _linearFilter, _clamp2edge, load_context);
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
}
