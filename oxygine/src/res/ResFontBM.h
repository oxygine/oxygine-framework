#pragma once
#include "oxygine_include.h"
#include "ResFont.h"
#include "core/Texture.h"
#include <vector>

namespace oxygine
{
	class Resources;
	class CreateResourceContext;

	DECLARE_SMART(NativeTexture, spNativeTexture);

	class ResFontBM: public ResFont
	{
	public:
		static Resource *create(CreateResourceContext &context);
		static Resource *createBM(CreateResourceContext &context);
		static Resource *createSD(CreateResourceContext &context);

		ResFontBM();
		~ResFontBM();

		/**use it only if you want create font without Resources*/
		void init(const char *path, const char *file, bool premultipliedAlpha = true, bool signedDistanceFont = false);
		
		void cleanup();

		Font *getFont(const char *name = 0, int size = 0) const;

	private:		
		void _load(LoadResourcesContext *);
		void _unload();
		void _createFont(CreateResourceContext *context, bool sd, bool bmc);

		struct page
		{
			string file;
			spNativeTexture texture;
		};

		typedef vector<page> pages;
		pages _pages;
		Font *_font;

		TextureFormat _format;

		string _file;
		bool _premultipliedAlpha;
	};
}