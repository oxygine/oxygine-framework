#include "Resources.h"
#include "Resource.h"
#include "pugixml/pugixml.hpp"
#include "core/files_io.h"
#include <algorithm>

#include "core/logging.h"

#include <stdio.h>
#include "CreateResourceContext.h"

#include "ResAnim.h"
#include "ResFont.h"
#include "utils/stringUtils.h"

#include "core/logging.h"
#include <stdarg.h>

//#define FS_LOG(...) log::warning(__VA_ARGS__)
#define FS_LOG(...) {}

namespace oxygine
{

	Resources::registeredResources Resources::_registeredResources;
	void Resources::registerResourceType(Resources::createResourceCallback creationCallback, const char *resTypeID)
	{
		registeredResource r;
		r.cb = creationCallback;
		strcpy(r.id, resTypeID);
				
		registeredResources::iterator it = lower_bound(_registeredResources.begin(), _registeredResources.end(), r.id, registeredResource::comparePred2);
		_registeredResources.insert(it, r);

		/*
		for (it = _registeredResources.begin(); it != _registeredResources.end(); ++it)
		{
			registeredResource *q = &(*it);
			int er =0 ;
		}
		*/
		
	}
	
	Resources::Resources()
	{
	}

	Resources::~Resources()
	{
		free();		
	}

	ResAnim *Resources::getResAnim(const string &id, error_policy ep)
	{
		return getT<ResAnim>(id, ep);
	}

	ResFont *Resources::getResFont(const string &id, error_policy ep)
	{
		return getT<ResFont>(id, ep);
	}

	void Resources::load(LoadResourcesContext *context, ResLoadedCallback cb)
	{
		for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
		{
			Resource *res = *i;
			//log::messageln("loading res: %s", res->getName().c_str());
			res->load(context);
			if (cb)
				cb(res);
		}
	}


	void Resources::unload()
	{
		for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
		{
			Resource *res = *i;
			res->unload();
		}
	}

	void Resources::free()
	{
		for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
		{
			Resource *res = (*i);
			delete res;
		}
		_resources.resize(0);

		for (size_t i = 0; i < _docs.size(); ++i)
		{
			delete _docs[i];
		}
		_docs.resize(0);
	}

	string getPath(const char *currentPath, const char *str)
	{
		string s;
		if (str[0] == '.' && (str[1] == '/' || str[1] == '\\'))
		{
			s = currentPath;
			s += str + 2;
			return s;
		}
		return str;
	}
	
	static bool findPred (const Resource *ob, const string &name)
	{
		return Resource::findPred(*ob, name);
	}

	static bool comparePred (const Resource *ob1, const Resource *ob2)
	{
		return Resource::comparePred(*ob1, *ob2);
	}

	void Resources::loadXML(const string &xml_name, LoadResourcesContext *load_context, bool load_completely, bool use_load_counter, const string &prebuilt_folder_)
	{
		_name = xml_name;

		string path;

		for (int i = xml_name.size() - 1; i >= 0; --i)
		{
			char c = xml_name[i];
			if (c == '\\' || c == '/')
			{
				path = xml_name.substr(0, i + 1);
			}
		}
		
		FS_LOG("step0");
		file::buffer fb;
		file::read(xml_name.c_str(), fb);

		FS_LOG("step1");
		

		char destHead[255];
		char destTail[255];
		path::split(xml_name.c_str(), destHead, destTail);

		string xml_only_name = destTail;

		string prebuilt_folder = prebuilt_folder_ + "/" + destTail + ".ox/";
		if (prebuilt_folder[0] == '/')
		{
			prebuilt_folder.erase(prebuilt_folder.begin());
		}

		file::buffer fb_meta;
		pugi::xml_document doc_meta;
		string ox = prebuilt_folder + "meta.xml";
		const char *ox_file = ox.c_str();

			

		FS_LOG("step2");

		if (file::exists(ox_file))
		{
			file::read(ox_file, fb_meta, ep_ignore_error);
			if (fb_meta.getSize())
				doc_meta.load_buffer_inplace(&fb_meta.data[0], fb_meta.data.size());
		}

		if (!fb.data.size())
		{
			OX_ASSERT(fb.data.size()  && "can't find xml file");
			return;
		}
		pugi::xml_document *doc = new pugi::xml_document();
		_docs.push_back(doc);

		bool loaded = doc->load_buffer(&fb.data[0], fb.data.size());
		OX_ASSERT(loaded);

		pugi::xml_node resources = doc->first_child();
		pugi::xml_node resources_meta = doc_meta.first_child();
		pugi::xml_node meta_node = resources_meta.first_child();


		int i = 0;

		string id;
		//string file;
		string rect_str;

		FS_LOG("loading xml resources");

		float scaleFactor = 1.0f;

		pugi::xml_node child = resources.first_child();
		while (!child.empty())
		{
			const char *name = child.name();
			pugi::xml_attribute attr;

			

			if (!strcmp(name, "set"))
			{
				pugi::xml_attribute attr = child.first_attribute();
				while (attr)
				{
					if (!strcmp(attr.name(), "path"))
					{
						path = getPath(path.c_str(), attr.value());
						path += "/";
					}
					if (!strcmp(attr.name(), "load"))
					{
						load_completely = attr.as_bool();
					}
					if (!strcmp(attr.name(), "scale_factor"))
					{
						scaleFactor = attr.as_float(scaleFactor);
					}
					attr = attr.next_attribute();
				}
			}
			else/*
			if (!strcmp(name, "atlas"))
			{
				loadAtlas(path, child, meta_node);
				meta_node = meta_node.next_sibling();
			}
			else*/
			{
				bool load = child.attribute("load").as_bool(load_completely);

				registeredResources::iterator i = lower_bound(_registeredResources.begin(), _registeredResources.end(), name);
				if (i != _registeredResources.end())
				{
					registeredResource &r = *i;
					if (!strcmp(r.id, name))
					{
						CreateResourceContext context;
						context.xml_name = &xml_name;
						context.node = child;
						context.meta = meta_node;
						context.folder = &path;
						context.resources = this;
						context.scale_factor = scaleFactor;

						string prebuilt_xml_folder = prebuilt_folder + "/" + name + "/";
						context.prebuilt_folder = &prebuilt_xml_folder;


						FS_LOG("resource: %s ", name);
						Resource *res = r.cb(context);
						OX_ASSERT(res);
						res->setUseLoadCounter(use_load_counter);

						meta_node = context.meta;

						if (res)
						{	
							//res-> = child;
							if (load)
								res->load(load_context);

							_resources.push_back(res);
						}
					}
					else
					{
						log::error("unknown resource. type: '%s' id: '%s'", name, Resource::extractID(child, "", "").c_str());
						OX_ASSERT(!"unknown resource type");
					}
				}
			}

			child = child.next_sibling();
		}

		sort(_resources.begin(), _resources.end(), comparePred);
		FS_LOG("xml loaded");
		//print();
	}

	void Resources::add(Resource *r)
	{
		OX_ASSERT(r);
		if (!r)
			return;
		/*
		OX_ASSERT(_resources[r->getName()] == 0);

		_resources[r->getName()] = r;
		*/

		//todo insert to correct place
		r->setName(lower(r->getName()));
		_resources.push_back(r);	
		//OX_ASSERT(0);
	}


	void Resources::print()
	{
		log::message("resources:\n");
		for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
		{
			Resource *res = *i;
			log::message("%s\n", res->getName().c_str());
		}
	}

	class ResPred
	{
	public:
		bool operator ()(const Resource *ob, const Resource *wtf)
		{
			return Resource::findPred(*ob, wtf->getName());			
		}
	};



	Resource *Resources::get(const string &id_, error_policy ep)
	{	
		string id = lower(id_);

#ifdef __S3E__
		resources::iterator it = lower_bound(_resources.begin(), _resources.end(), id, findPred);
#else
		ResAnim r;
		r.setName(id);
		ResPred pr;
		resources::iterator it = lower_bound(_resources.begin(), _resources.end(), &r, pr);
#endif

		if (it != _resources.end())
		{
			if ((*it)->getName() == id)
				return (*it);
		}

		
		handleErrorPolicy(ep, "can't find resource: '%s' in '%s'", id.c_str(), _name.c_str());
		return 0;
	}
}