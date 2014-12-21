#pragma once
#include "oxygine_include.h"
#include "core/Object.h"
#include "pugixml/pugixml.hpp"
#include "res/ResAnim.h"
#include "res/ResFont.h"
#include "utils/stringUtils.h"

namespace oxygine
{
	DECLARE_SMART(Actor, spActor);

	class Resource;
	class ResAnim;
	class ResFont;

	class creator
	{
	public:
		virtual spActor		create(const char *type) const;
		virtual Resource*	getResource(const char *id) const {return 0;}
		virtual ResAnim*	getResAnim(const char *id) const {return safeCast<ResAnim*>(getResource(id));}
		virtual ResFont*	getResFont(const char *id) const {return safeCast<ResFont*>(getResource(id));}
	};

	struct serializedata
	{
		serializedata() :withChildren(true){}
		pugi::xml_node node;
		bool withChildren;
	};

	struct deserializedata
	{
		pugi::xml_node node;
		const creator *factory;
		static spActor deser(pugi::xml_node node, const creator* factory);
	};
	

	inline void setAttrV2(pugi::xml_node node, const char *name, const Vector2 &v, const Vector2 &def)
	{
		if (v == def)
			return;
		char str[255];
		safe_sprintf(str, "%g,%g", v.x, v.y);
		node.append_attribute(name).set_value(str);
	}


	template<class T>
	void setAttr(pugi::xml_node node, const char *name, T v, T def)
	{
		if (v == def)
			return;
		node.append_attribute(name).set_value(v);
	}
}