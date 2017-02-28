#pragma once
#include "oxygine-include.h"
#include "STDRenderer.h"
#include "Material.h"

namespace oxygine
{
    DECLARE_SMART(STDMaterial, spSTDMaterial);
    class STDMaterial : public Material
    {
    public:
        static STDMaterial* instance;

        STDMaterial(STDRenderer* r): _renderer(r) {}

        void apply(Material* prev) override;
        void finish() override;

        void render(ClipRectActor*, const RenderState& rs) override;
        void render(MaskedSprite*, const RenderState& rs) override;
        void doRender(Sprite*, const RenderState& rs) override;
        void doRender(TextField*, const RenderState& rs) override;
        void doRender(ColorRectSprite*, const RenderState& rs) override;
        void doRender(ProgressBar*, const RenderState& rs) override;

        void setViewProj(const Matrix& vp);

        STDRenderer* getRenderer() { return _renderer; }

    protected:
        STDRenderer* _renderer;
    };

}