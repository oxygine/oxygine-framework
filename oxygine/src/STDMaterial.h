#pragma once
#include "STDRenderer.h"
#include "Material.h"
namespace oxygine
{
    class ClipRectActor;
    class Sprite;
    class TextField;
    class ColorRectSprite;

    class STDMaterial : public Material
    {
    public:
        static STDMaterial* instance;

        STDMaterial(STDRenderer* r): _renderer(r) {}

        void apply(Material* prev) OVERRIDE;
        void finish() OVERRIDE;

        void render(ClipRectActor*, const RenderState& rs) OVERRIDE;
        void render(MaskedSprite*, const RenderState& rs) OVERRIDE;
        void doRender(Sprite*, const RenderState& rs) OVERRIDE;
        void doRender(TextField*, const RenderState& rs) OVERRIDE;
        void doRender(ColorRectSprite*, const RenderState& rs) OVERRIDE;
        void doRender(ProgressBar*, const RenderState& rs) OVERRIDE;

        void setViewProj(const Matrix& vp);

        STDRenderer* getRenderer() { return _renderer; }

    protected:
        //STDRenderer _renderer_;
        STDRenderer* _renderer;
    };

}