#include "Resource.h"
#include <stdio.h>
#include "utils/stringUtils.h"

namespace oxygine
{
	Resource::Resource():_loadCounter(0), _useLoadCounter(false)
	{

	}

	Resource::~Resource()
	{

	}

	void Resource::load(LoadResourcesContext *context /* = 0 */)
	{
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

	string Resource::extractID(const pugi::xml_node &node, const string &file, const string &def)
	{	
		string id = node.attribute("id").value();
		if (id.empty())
		{
			if (file.empty())
				return lower(def);
			char head[255];
			char tail[255];
			path::split(file.c_str(), head, tail);

			char name[255];
			char ext[255];
			path::splitExt(tail, name, ext); 
			return lower(name);
		}

		return lower(id);
	}
	 
	
	pugi::xml_attribute Resource::getAttribute(const char *attr) const
	{
		return _node.attribute(attr);
	}
}