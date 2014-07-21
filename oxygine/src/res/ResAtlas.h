#pragma once
#include "oxygine_include.h"
#include "core/file.h"
#include <map>
#include "Resource.h"
#include "ResAnim.h"

namespace pugi
{
	class xml_node;
}

namespace oxygine
{
	using namespace std;

	class Resources;
	class CreateResourceContext;
	DECLARE_SMART(NativeTexture, spNativeTexture);

	class ResAtlas: public Resource
	{
	public:
		static Resource *create(CreateResourceContext &context);
		struct atlas
		{
			spNativeTexture base;
			string base_path;

			spNativeTexture alpha;
			string alpha_path;
		};


		ResAtlas();
		~ResAtlas();

		void addAtlas(TextureFormat tf, const string &base, const string &alpha, int w, int h);
		const atlas& getAtlas(int i) const {return _atlasses[i];}
		int getNum() const { return (int)_atlasses.size(); }

	protected:
		void _restore(Restorable *r, void *user);

		void _load(LoadResourcesContext *);
		void _unload();

		void loadAtlas(CreateResourceContext &context);
		static void init_resAnim(ResAnim *rs, const string &file, pugi::xml_node node);

	private:		

		typedef vector<atlas> atlasses;
		atlasses _atlasses;
	};

	typedef void (*load_texture_hook)(const string &file, spNativeTexture nt, LoadResourcesContext *load_context);
	void set_load_texture_hook(load_texture_hook);
}