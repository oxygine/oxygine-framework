#include "CreateResourceContext.h"
#include "core/NativeTexture.h"
#include "MemoryTexture.h"

namespace oxygine
{	
	XmlWalker::XmlWalker(const std::string &folder, float scaleFactor, bool load, pugi::xml_node xml, pugi::xml_node meta) :_rootMeta(meta),
		_root(xml),
		_notStarted(true), 
		_notStartedMeta(true),
		_scaleFactor(scaleFactor),
		_load(load),
		_base(folder),
		_path(folder)
	{
		//_scaleFactor = _root.attribute("scale_factor").as_float(_scaleFactor);
	}

	std::string XmlWalker::connectPath(const char *currentPath, const char *str)
	{
		std::string s;
		if (str[0] == '.' && (str[1] == '/' || str[1] == '\\'))
		{
			s = currentPath;
			s += str + 2;
			return s;
		}
		return str;
	}


	std::string XmlWalker::getPath(const char *attrName) const
	{
		const char *str = _root.attribute(attrName).as_string();
		if (str[0] == '.' && (str[1] == '/' || str[1] == '\\'))
		{
			str += 2;
		}

		return _path + str;
	}

	XmlWalker XmlWalker::next()
	{
		while (1)
		{
			if (_notStarted)
			{
				_last = _root.first_child();
				_notStarted = false;
			}
			else
			{
				_last = _last.next_sibling();
			}

			nextMeta();


			const char *name = _last.name();
			pugi::xml_attribute attr;

#ifdef OX_DEBUG
			if (!_rootMeta.empty())
			{
				//OX_ASSERT(!strcmp(_last.name(), _lastMeta.name()));
			}			
#endif

			if (!strcmp(name, "set"))
			{
				_checkSetAttributes(_last);				
				continue;
			}

			break;
		}

		return XmlWalker(_path, _scaleFactor, _load, _last, _lastMeta);
	}

	void XmlWalker::_checkSetAttributes(pugi::xml_node node)
	{
		pugi::xml_attribute attr = node.first_attribute();
		while (attr)
		{
			if (!strcmp(attr.name(), "path"))
			{
				_path = connectPath(_base.c_str(), attr.value());
				if (!_path.empty())
					_path += "/";
			} else
				if (!strcmp(attr.name(), "load"))
				{
					_load = attr.as_bool();
				} else
					if (!strcmp(attr.name(), "scale_factor"))
					{
						_scaleFactor = attr.as_float(1.0f);
					}

			attr = attr.next_attribute();
		}	
	}

	void XmlWalker::checkSetAttributes()
	{
		_checkSetAttributes(_root);
	}

	pugi::xml_node XmlWalker::nextMeta()
	{
		if (_notStartedMeta)
		{
			_lastMeta = _rootMeta.first_child();
			_notStartedMeta = false;
		}
		else		
			_lastMeta = _lastMeta.next_sibling();
		

		return _lastMeta;
	}

	RestoreResourcesContext RestoreResourcesContext::instance;
	void RestoreResourcesContext::createTexture(spMemoryTexture src, spNativeTexture dest)
	{
		dest->init(src->lock(), false);
	}

	bool RestoreResourcesContext::isNeedProceed(spNativeTexture t)
	{
		return true;
	}

	SingleThreadResourcesContext SingleThreadResourcesContext::instance;

	void SingleThreadResourcesContext::createTexture(spMemoryTexture src, spNativeTexture dest)
	{
		dest->init(src->lock(), false);
	}

	bool SingleThreadResourcesContext::isNeedProceed(spNativeTexture t)
	{
		return t->getHandle() == 0;
	}
}
