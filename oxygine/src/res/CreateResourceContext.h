#pragma once
#include <string>
#include "pugixml/pugixml.hpp"


namespace oxygine
{
	class Resources;
	/**internal class*/
	class CreateResourceContext//todo rename
	{
	public:
		CreateResourceContext():
			resources(0), scale_factor(1.0f), xml_name(0), folder(0), prebuilt_folder(0)
		{}

		Resources *resources;
		float scale_factor;
		
		pugi::xml_node node;
		pugi::xml_node meta;

		const string *xml_name;
		const string *folder;
		const string *prebuilt_folder;
	};

	DECLARE_SMART(MemoryTexture, spMemoryTexture);
	DECLARE_SMART(NativeTexture, spNativeTexture);
	
	/**internal class*/
	class LoadResourcesContext
	{
	public:
		virtual ~LoadResourcesContext(){}
		virtual void createTexture(spMemoryTexture src, spNativeTexture dest) = 0;
	};
}