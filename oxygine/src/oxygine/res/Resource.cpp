#include "Resource.h"
#include "CreateResourceContext.h"
#include "../utils/stringUtils.h"
#include <stdio.h>

namespace oxygine
{
    Resource::Resource(): _loadCounter(0), _useLoadCounter(false), _parent(0)
    {

    }

    Resource::~Resource()
    {

    }

    void Resource::load(LoadResourcesContext* context /* = 0 */)
    {
        if (!context)
            context = LoadResourcesContext::get();

        if (_loadCounter == 0)
            _load(context);

        _loadCounter = _useLoadCounter ? _loadCounter + 1 : 1;
    }

    void Resource::unload()
    {
        if (_loadCounter == 1)
            _unload();

        _loadCounter = _useLoadCounter ? _loadCounter - 1 : 0;
    }

    std::string Resource::extractID(const pugi::xml_node& node, const std::string& file, const std::string& def)
    {
        std::string id = node.attribute("id").value();
        if (id.empty())
        {
            if (file.empty())
                return lower(def);
            return extractID(file);
        }

        return lower(id);
    }

    std::string Resource::extractID(const std::string& file)
    {
        char head[255];
        char tail[255];
        path::split(file.c_str(), head, tail);

        char name[255];
        char ext[255];
        path::splitExt(tail, name, ext);
        return lower(name);
    }


    pugi::xml_attribute Resource::getAttribute(const char* attr) const
    {
        return _node.attribute(attr);
    }
}