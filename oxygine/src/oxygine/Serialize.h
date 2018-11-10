#pragma once
#include "oxygine-include.h"
#include "core/Object.h"
#include "pugixml/pugixml.hpp"
#include "res/ResAnim.h"
#include "res/ResFont.h"
#include "utils/stringUtils.h"

namespace oxygine
{
    class creator
    {
    public:
        virtual spActor     create(const char* type) const;
        virtual const Resource*   getResource(const char* id) const {return 0;}
        virtual const ResAnim*    getResAnim(const char* id) const {return safeCast<const ResAnim*>(getResource(id));}
        virtual AnimationFrame getFrame(const char* id, int col, int row) const { const ResAnim* rs = getResAnim(id);  if (rs) return rs->getFrame(col, row); return AnimationFrame();}
        virtual const ResFont*    getResFont(const char* id) const {return safeCast<const ResFont*>(getResource(id));}
        virtual bool              getString(const char* id, std::string &result) const { return false; }
    };

    struct serializedata
    {
        serializedata() : withChildren(true) {}
        pugi::xml_node node;
        bool withChildren;
    };

    struct deserializedata
    {
        pugi::xml_node node;
        const creator* factory;

        static spActor deser(pugi::xml_node node, const creator* factory);
    };

    struct deserializeLinkData
    {
        pugi::xml_node node;
        spActor root;

        static void link(pugi::xml_node node, spActor root);
    };


    inline void setAttrV2(pugi::xml_node node, const char* name, const Vector2& v, const Vector2& def)
    {
        if (v == def)
            return;
        char str[255];
        safe_sprintf(str, "%g,%g", v.x, v.y);
        node.append_attribute(name).set_value(str);
    }


    template<class T>
    void setAttr(pugi::xml_node node, const char* name, T v, T def)
    {
        if (v == def)
            return;
        node.append_attribute(name).set_value(v);
    }
}
