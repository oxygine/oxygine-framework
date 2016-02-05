#pragma once
#include "oxygine_include.h"
#include "ResFont.h"
#include "core/Texture.h"
#include <vector>

namespace oxygine
{
    class Resources;
    class CreateResourceContext;
    class Restorable;

    DECLARE_SMART(NativeTexture, spNativeTexture);

    class ResFontBM: public ResFont
    {
    public:
        static Resource* create(CreateResourceContext& context);
        static Resource* createBM(CreateResourceContext& context);
        static Resource* createSD(CreateResourceContext& context);

        ResFontBM();
        ~ResFontBM();

        /**use it only if you want create font without Resources*/
        void init(const char* fntPath, bool premultipliedAlpha = true);

        void cleanup();

        const Font* getFont(const char* name = 0, int size = 0) const OVERRIDE;

    private:
        struct page
        {
            std::string file;
            spNativeTexture texture;
        };

        void addPage(int tw, int th, const char* head, const char* file);

        void _loadPage(const page& p, LoadResourcesContext*);
        void _load(LoadResourcesContext*);
        void _unload();
        void _restore(Restorable*, void*);
        void _createFont(CreateResourceContext* context, bool sd, bool bmc);
        void _createFontFromTxt(CreateResourceContext* context, char* fontData, const std::string& fontPath);

        typedef std::vector<page> pages;
        pages _pages;
        Font* _font;

        TextureFormat _format;

        std::string _file;
        bool _premultipliedAlpha;
    };
}
