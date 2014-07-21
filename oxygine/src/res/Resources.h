#pragma once
#include "oxygine_include.h"
#include <string>
#include <list>
#include <string.h>
#include "pugixml/pugixml.hpp"
#include "closure/closure.h"
#include "core/Object.h"
#include "Resource.h"

namespace pugi
{
	class xml_node;
}

namespace oxygine
{
	using namespace std;
	
	class Resource;
	class ResFont;
	class ResAnim;

	class CreateResourceContext;
	class LoadResourcesContext; 


	class Resources: public Resource
	{
	public:
		typedef vector<spResource> resources;
		typedef Resource* (*createResourceCallback)(CreateResourceContext &context);
		typedef Closure<void (Resource *)> ResLoadedCallback;

		/**Registers your own resource type. 
		@param creation callback
		@param resource type string identifier. Max id length is 15 chars. These IDs are already occupied: 'set', 'atlas', ' image', 'font', 'buffer'
		*/
		static void registerResourceType(createResourceCallback creationCallback, const char *resTypeID);
		static void unregisterResourceType(const char *resTypeID);

		
		Resources();
		~Resources();

		/**Loads resources from xml file.
		@param xml file path
		@param used for multithreading loading
		@param should be each resource loaded completely includes internal heavy data (atlasses/textures/buffers) or load only their definition. Could be overloaded in xml: <your_res_type ... load = "false"/>
		@param use load counter internally
		@param use not standard folder with prebuilt resources (atlasses, fonts, etc)
		*/
		void loadXML(const string &file, LoadResourcesContext *load_context = 0, 
			bool load_completely = true, bool use_load_counter = false,
			const string &prebuilt_folder = "");

        /**Adds your own Resource and becomes resource owner if Own is true. Owned resource will be deleted from destructor by calling 'delete'.*/
		void add(Resource *r);
		
		/**Calls Resource::load for each resoure in the list*/
		void load(LoadResourcesContext *context = 0, ResLoadedCallback cb = ResLoadedCallback());

		/**Unloads data from memory, all resources handles remain valid*/
		void unload();			

		/**Completely deletes all resources*/
		void free();

		/** get resource by id, no case sensitive   
		@param resource id
		@param if showError is true and resource is missing warning/assert will appear
		*/
		Resource *get(const string &id, error_policy ep = ep_show_error);

		/** returns resource by index */
		Resource *get(int index){return _fastAccessResources[index].get();}
		int		  getCount() const {return (int)_fastAccessResources.size();}

		Resource * operator[](const string &id){return get(id);}
		
		/** get resource by id
		@param resource id
		@param if safe is true and resource is missing warning/assert will appear
		*/
		ResAnim *getResAnim(const string &id, error_policy ep = ep_show_error);
		 
		/** get animation resource by id
		@param resource id
		@param if safe is true and resource is missing warning/assert will appear
		*/
		ResFont *getResFont(const string &id, error_policy ep = ep_show_error);

		template<class T>
		T *getT(const string &id, error_policy ep = ep_show_error){return safeCast<T*> (get(id, ep));}

		/**sorting manually added resources*/
		void sort();
		/**debug function. prints all loaded resources*/
		void print();		


		resources& _getResources();

	protected:
        void updateName(const string &filename);
		void _load(LoadResourcesContext *context);
		void _unload();


		struct registeredResource
		{
			registeredResource(){id[0]=0;}
			char id[16];
			createResourceCallback cb;

			static bool comparePred2(const registeredResource &ob1, const char *ob)
			{
				return strcmp(ob1.id, ob) > 0;
			}

			static bool comparePred (const registeredResource &ob1, const registeredResource &ob2)
			{
				return strcmp(ob1.id, ob2.id) > 0;
			}

			bool operator < (const char *ob2) const
			{
				return strcmp(this->id, ob2) > 0;
			}
		};

		
		resources _resources;
		//resources _owned;
		resources _fastAccessResources;

		typedef vector< registeredResource > registeredResources;
		static registeredResources _registeredResources;
		
		string _name;
		std::vector<pugi::xml_document*> _docs;
	};
}
