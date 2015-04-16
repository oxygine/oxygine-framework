#pragma once
#include "oxygine_include.h"
#include "core/Renderer.h"
namespace oxygine
{
    class STDRenderer : public Renderer
    {
    public:
        STDRenderer(IVideoDriver* driver = 0);

        /**Sets blend mode. Default value is blend_premultiplied_alpha*/
        void draw(const RState* rs, const Color&, const RectF& srcRect, const RectF& destRect) OVERRIDE;

        void setUberShaderProgram(UberShaderProgramBase* pr);

        void setBlendMode(blend_mode blend);
        /**Sets texture. If texture is null White texture would be used.*/
        void setTexture(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied = true);


        //debug utils
#ifdef OXYGINE_DEBUG_T2P
        static void showTexel2PixelErrors(bool show);
#endif

    protected:

        void preDrawBatch();
        void _cleanup();
        void _begin();
        void _resetSettings();

        spNativeTexture _base;
        spNativeTexture _alpha;

        blend_mode _blend;

        UberShaderProgramBase* _uberShader;
        unsigned int _shaderFlags;
    };


    class PrimitiveRenderer
    {
    public:
        PrimitiveRenderer(STDRenderer* r);

        void setTexture(spNativeTexture texture);
        void setBlendMode(blend_mode mode);
        void draw(const void* data, int size, bvertex_format format, bool worldTransform = false);

    protected:
        STDRenderer* _renderer;
    };

    class TextRenderer2
    {
    public:
        TextRenderer2(STDRenderer*);

        void draw(const AffineTransform& tr, spNativeTexture texture, unsigned int color, const RectF& src, const RectF& dest);

    protected:
        spNativeTexture _texture;
        STDRenderer* _renderer;
    };
}