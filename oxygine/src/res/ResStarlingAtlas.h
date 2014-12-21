#pragma once
#include "oxygine_include.h"
#include "Resource.h"
namespace oxygine
{
	class Resources;
	class Restorable;
	class CreateResourceContext;
	DECLARE_SMART(NativeTexture, spNativeTexture);

	class ResStarlingAtlas: public Resource
	{
	public:
		static Resource *create(CreateResourceContext &context);

		ResStarlingAtlas();
		~ResStarlingAtlas();

		void init(Resources *, pugi::xml_node &node);
		

		void mtload();

	private:
		void loadAtlas(CreateResourceContext &context);
		void _load(LoadResourcesContext *);
		void _unload();
		void _restore(Restorable *r, void *);

		spNativeTexture _texture;
		std::string _imagePath;
	};
}