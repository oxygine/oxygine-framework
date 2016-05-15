#include "Resources.h"
#include "Resource.h"
#include "pugixml/pugixml.hpp"
#include "core/file.h"
#include <algorithm>

#include "core/log.h"

#include <stdio.h>
#include "CreateResourceContext.h"

#include "ResAnim.h"
#include "ResFont.h"
#include "utils/stringUtils.h"

#include "core/log.h"
#include <stdarg.h>

//#define FS_LOG(...) log::warning(__VA_ARGS__)
#define FS_LOG(...) {}

namespace oxygine
{

    Resources::registeredResources Resources::_registeredResources;
    void Resources::registerResourceType(Resources::createResourceCallback creationCallback, const char* resTypeID)
    {
        registeredResource r;
        r.cb = creationCallback;
        strcpy(r.id, resTypeID);

        registeredResources::iterator it = std::lower_bound(_registeredResources.begin(), _registeredResources.end(), r.id, registeredResource::comparePred2);
        if (it != _registeredResources.end())
        {
            if (!strcmp(it->id, resTypeID))
            {
                OX_ASSERT(!"resource already registered");
                return;
            }
        }
        _registeredResources.insert(it, r);

        /*
        for (it = _registeredResources.begin(); it != _registeredResources.end(); ++it)
        {
            registeredResource *q = &(*it);
            int er =0 ;
        }
        */
    }

    void Resources::unregisterResourceType(const char* resTypeID)
    {
        registeredResources::iterator it = std::lower_bound(_registeredResources.begin(), _registeredResources.end(), resTypeID, registeredResource::comparePred2);
        if (it != _registeredResources.end())
        {
            if (!strcmp(it->id, resTypeID))
            {
                _registeredResources.erase(it);
                return;
            }
        }
        OX_ASSERT(!"can't find resource type");
    }

    Resources::Resources()
    {
    }

    Resources::~Resources()
    {
        free();
    }

    ResAnim* Resources::getResAnim(const std::string& id, error_policy ep) const
    {
        return getT<ResAnim>(id, ep);
    }

    ResFont* Resources::getResFont(const std::string& id, error_policy ep) const
    {
        return getT<ResFont>(id, ep);
    }

    void Resources::load(ResLoadedCallback cb)
    {
        _Resource::load(0);
        //if (cb)
        //  cb(thi)
    }

    void Resources::unload()
    {
        _Resource::unload();
    }

    void Resources::_load(LoadResourcesContext* context)
    {
        for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
        {
            Resource* res = (*i).get();
            //log::messageln("loading res: %s", res->getName().c_str());
            res->load(context);
            //if (cb)
            //  cb(res);
        }
    }

    void Resources::_unload()
    {
        for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
        {
            Resource* res = (*i).get();
            res->unload();
        }
    }

    void Resources::free()
    {
        _resourcesMap.clear();
        _resources.clear();

        for (size_t i = 0; i < _docs.size(); ++i)
            delete _docs[i];
        _docs.clear();

        __freeName();
    }

    void Resources::updateName(const std::string& filename)
    {
        char head[256];
        char tail[256];
        path::split(filename.c_str(), head, tail);

        setName(tail);
    }


    class ObjectBasePredicate
    {
    public:
        bool operator()(const ObjectBase* res, const char* name) const
        {
            return strcmp(res->getName().c_str(), name) < 0;
        }

        bool operator()(const spResource& res, const char* name) const
        {
            return strcmp(res->getName().c_str(), name) < 0;
        }

        bool operator()(const spResource& resA, const spResource& resB) const
        {
            return strcmp(resA->getName().c_str(), resB->getName().c_str()) < 0;
        }

        bool operator()(const char* name, const ObjectBase* res) const
        {
            return strcmp(res->getName().c_str(), name) < 0;
        }

        bool operator()(const ObjectBase* resA, const ObjectBase* resB) const
        {
            return strcmp(resA->getName().c_str(), resB->getName().c_str()) < 0;
        }
    };


    void Resources::loadXML(const std::string& xmlFile, const ResourcesLoadOptions& opt)
    {
        _name = xmlFile;
        _loadCounter = opt._loadCompletely ? 1 : 0;


        FS_LOG("step0");
        file::buffer fb;
        file::read(xmlFile.c_str(), fb);

        FS_LOG("step1");


        updateName(xmlFile);

        char destHead[255];
        char destTail[255];
        path::split(xmlFile.c_str(), destHead, destTail);

        std::string prebuilt_folder = opt._prebuilFolder + "/" + destTail + ".ox/";
        if (prebuilt_folder[0] == '/')
        {
            prebuilt_folder.erase(prebuilt_folder.begin());
        }

        file::buffer fb_meta;
        pugi::xml_document doc_meta;
        std::string ox = prebuilt_folder + "meta.xml";
        const char* ox_file = ox.c_str();


        FS_LOG("step2");

        if (file::exists(ox_file))
        {
            file::read(ox_file, fb_meta, ep_ignore_error);
            if (fb_meta.getSize())
                doc_meta.load_buffer_inplace(&fb_meta.data[0], fb_meta.data.size());
        }

        if (!fb.data.size())
        {
            OX_ASSERT(fb.data.size() && "can't find xml file");
            return;
        }

        pugi::xml_document* doc = new pugi::xml_document();
        _docs.push_back(doc);

        bool loaded = doc->load_buffer(&fb.data[0], fb.data.size());
        OX_ASSERT(loaded);

        pugi::xml_node resources = doc->first_child();
        pugi::xml_node resources_meta = doc_meta.first_child();
        if (!resources_meta.empty())
        {
            int metaVersion = resources_meta.attribute("version").as_int(0);
            OX_ASSERT(metaVersion <= 2 && "Incompatible atlas format. Please rebuild xmls with latest 'oxyresbuild' tool or delete .ox folder from data.");
        }


        std::string id;

        FS_LOG("loading xml resources");

        std::string xmlFolder = destHead;
        XmlWalker walker(&xmlFolder, "", 1.0f, opt._loadCompletely, true, resources, resources_meta);

        while (true)
        {
            CreateResourceContext context;
            context.options = &opt;
            context.walker = walker.next();
            if (context.walker.empty())
                break;


            const char* type = context.walker.getType();

            registeredResources::iterator i = std::lower_bound(_registeredResources.begin(), _registeredResources.end(), type);
            if (i == _registeredResources.end() || strcmp(i->id, type))
            {
                log::error("unknown resource. type: '%s' id: '%s'", type, _Resource::extractID(context.walker.getNode(), "", "").c_str());
                OX_ASSERT(!"unknown resource type");
                continue;
            }

            registeredResource& r = *i;


            context.xml_name = &xmlFile;
            context.resources = this;

            std::string prebuilt_xml_folder = prebuilt_folder + type + "/";
            context.prebuilt_folder = &prebuilt_xml_folder;


            FS_LOG("resource: %s ", name);
            Resource* res = r.cb(context);
            OX_ASSERT(res);
            res->setUseLoadCounter(opt._useLoadCounter);

            if (res)
            {
                if (context.walker.getLoad())
                    res->load(0);
                res->setParent(this);
                _resources.push_back(res);
            }
        }

        FS_LOG("xml loaded");
    }

    void Resources::collect(resources& r)
    {
#ifdef __S3E__
        for (resourcesMap::const_iterator i = _resourcesMap.begin(); i != _resourcesMap.end(); ++i)
#else
        for (resourcesMap::const_iterator i = _resourcesMap.cbegin(); i != _resourcesMap.cend(); ++i)
#endif
        {
            spResource res = i->second;
            r.push_back(res);
        }

    }

    void Resources::add(Resource* r, bool accessByShortenID)
    {
        OX_ASSERT(r);
        if (!r)
            return;

        std::string name = lower(r->getName());
        r->setName(name);
        _resourcesMap[name] = r;

        if (accessByShortenID)
        {
            std::string shortName = path::extractFileName(name);
            _resourcesMap[shortName] = r;
        }
    }


    void Resources::print() const
    {
        log::message("resources:\n");
#ifdef __S3E__
        for (resourcesMap::const_iterator i = _resourcesMap.begin(); i != _resourcesMap.end(); ++i)
#else
        for (resourcesMap::const_iterator i = _resourcesMap.cbegin(); i != _resourcesMap.cend(); ++i)
#endif

        {
            spResource res = i->second;
            log::message("%s\n", res->getName().c_str());
        }

        /*
        unsigned n = _resourcesMap.bucket_count();

        for (unsigned i=0; i<n; ++i) {
            log::message("bucket %d: ", i);
            for (auto it = _resourcesMap.begin(i); it!=_resourcesMap.end(i); ++it)
                log::message("%s, ", it->first.c_str());
            log::messageln(" ");
        }
        */
    }

    Resources::resources& Resources::_getResources()
    {
        return _resources;
    }

    Resource* Resources::get(const std::string& id_, error_policy ep) const
    {
        std::string id = lower(id_);

        resourcesMap::const_iterator it = _resourcesMap.find(id);

        if (it != _resourcesMap.end())
        {
            return it->second.get();
        }

        handleErrorPolicy(ep, "can't find resource: '%s' in '%s'", id.c_str(), _name.c_str());
        return 0;
    }
}
