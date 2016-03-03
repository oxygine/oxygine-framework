#pragma once
#include "oxygine_include.h"
#include <string>
#include <list>
#include <string.h>
#include "pugixml/pugixml.hpp"
#include "closure/closure.h"
#include "core/Object.h"
#include "Resource.h"
#ifdef __S3E__
#include <map>
#else
#include <unordered_map>
#endif

namespace pugi
{
    class xml_node;
}

namespace oxygine
{
    class Resource;
    class ResFont;
    class ResAnim;

    class CreateResourceContext;
    class LoadResourcesContext;

    class ResourcesLoadOptions
    {
    public:
        ResourcesLoadOptions() : _loadCompletely(true), _useLoadCounter(false), _shortenIDS(false) {};

		//load only Resources definitions. Skips internal heavy data (atlasses/textures/buffers). Could be overridden in xml: <your_res_type ... load = "false"/>
		ResourcesLoadOptions& dontLoadAll(bool v = false) { _loadCompletely = v; return *this; }

		//use load counter internally
		ResourcesLoadOptions& useLoadCounter(bool v = true) { _useLoadCounter = v; return *this; }

		//use not standard folder with prebuilt resources (atlasses, fonts, etc)
		ResourcesLoadOptions& prebuiltFolder(const std::string &folder) {_prebuilFolder = folder; return *this; }


        bool _loadCompletely;
        bool _useLoadCounter;
        bool _shortenIDS;
        std::string _prebuilFolder;
    };

    class Resources: public _Resource
    {
    public:
        typedef std::vector<spResource> resources;
        typedef Resource* (*createResourceCallback)(CreateResourceContext& context);
        typedef Closure<void (Resource*)> ResLoadedCallback;

        /**Registers your own resource type.
        @param creation callback
        @param resource type string identifier. Max id length is 15 chars. These IDs are already occupied: 'set', 'atlas', ' image', 'font', 'buffer'
        */
        static void registerResourceType(createResourceCallback creationCallback, const char* resTypeID);
        static void unregisterResourceType(const char* resTypeID);

        Resources();
        ~Resources();

        /**
        DEPRECATED, USE loadXML2 method
        Loads resources from xml file.
        @param xml file path
        @param used for multi threading loading
        @param should be each resource loaded completely includes internal heavy data (atlasses/textures/buffers) or load only their definition. Could be overloaded in xml: <your_res_type ... load = "false"/>
        @param use load counter internally
        @param use not standard folder with prebuilt resources (atlasses, fonts, etc)
        */
		OXYGINE_DEPRECATED
        void loadXML(const std::string& file, LoadResourcesContext* load_context = 0,
                     bool load_completely = true, bool use_load_counter = false,
                     const std::string& prebuilt_folder = "");


        /**Loads resources from xml file. Load could be called multiple times for different xml files.
        @param xml file paths
        @param options
        */
        void loadXML2(const std::string& xmlFile, const ResourcesLoadOptions& opt = ResourcesLoadOptions());

		//use loadXML2, later it will be renamed to loadXML
		OXYGINE_DEPRECATED
		void load(const std::string& xmlFile, const ResourcesLoadOptions& opt = ResourcesLoadOptions());

        /**Adds Resource*/
        void add(Resource* r, bool accessByShortenID = false);

        /**Calls Resource::load for each resource in the list*/
        void load(ResLoadedCallback cb = ResLoadedCallback());

        /**Unloads data from memory, all resources handles remain valid*/
        void unload();

        /**Completely deletes all loaded resources*/
        void free();

        /** get resource by id, no case sensitive
        @param resource id
        */
        Resource* get(const std::string& id, error_policy ep = ep_show_error) const;

        /** returns resource by index */
        //Resource* get(int index) const {return _fastAccessResources.at(index).get();}
        //int       getCount() const {return (int)_fastAccessResources.size();}

        Resource* operator[](const std::string& id) { return get(id); }

        /** get resource by id
        @param resource id
        */
        ResAnim* getResAnim(const std::string& id, error_policy ep = ep_show_error) const;

        /** get animation resource by id
        @param resource id
        */
        ResFont* getResFont(const std::string& id, error_policy ep = ep_show_error) const;

        template<class T>
        T* getT(const std::string& id, error_policy ep = ep_show_error) const { return safeCast<T*>(get(id, ep)); }

        /**debug function. prints all loaded resources*/
        void print() const;

        /**collects all resources into vector*/
        void collect(resources&);

        void addShortIDS();

        resources& _getResources();

    protected:
        void updateName(const std::string& filename);
        void _load(LoadResourcesContext* context);
        void _unload();


        struct registeredResource
        {
            registeredResource() {id[0] = 0;}
            char id[16];
            createResourceCallback cb;

            static bool comparePred2(const registeredResource& ob1, const char* ob)
            {
                return strcmp(ob1.id, ob) > 0;
            }

            static bool comparePred(const registeredResource& ob1, const registeredResource& ob2)
            {
                return strcmp(ob1.id, ob2.id) > 0;
            }

            bool operator < (const char* ob2) const
            {
                return strcmp(this->id, ob2) > 0;
            }
        };


        resources _resources;
#ifdef __S3E__
        typedef std::map<std::string, spResource> resourcesMap;
#else
        typedef std::unordered_map<std::string, spResource> resourcesMap;
#endif
        resourcesMap _resourcesMap;


        typedef std::vector< registeredResource > registeredResources;
        static registeredResources _registeredResources;

        std::string _name;
        std::vector<pugi::xml_document*> _docs;
    };
}
