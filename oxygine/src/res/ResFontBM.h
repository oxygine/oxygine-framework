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

        /**loads "fnt" font from file, supported XML and text format*/
        void init(const char* fntPath, bool premultipliedAlpha = false);
        void initSD(const char* fntPath, int downsample);

        void cleanup();

        const Font* getFont(const char* name = 0, int size = 0) const OVERRIDE;

        bool isSDF(int& size) const OVERRIDE;

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
        void _createFont(CreateResourceContext* context, bool sd, bool bmc, int downsample);
        void _createFontFromTxt(CreateResourceContext* context, char* fontData, const std::string& fontPath, int downsample);

        typedef std::vector<page> pages;
        pages _pages;
        Font* _font;
        bool _sdf;

        TextureFormat _format;

        std::string _file;
        bool _premultipliedAlpha;
    };
}
