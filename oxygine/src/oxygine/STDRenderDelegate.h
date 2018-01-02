#pragma once
#include "oxygine-include.h"
#include "RenderDelegate.h"
#include "STDRenderer.h"

namespace oxygine
{
    DECLARE_SMART(STDRenderDelegate, spSTDMaterial);
    class STDRenderDelegate : public RenderDelegate
    {
    public:
        static STDRenderDelegate* instance;

        STDRenderDelegate() {}

        void render(ClipRectActor*,     const RenderState& rs) override;
        void render(MaskedSprite*,      const RenderState& rs) override;
        void doRender(Sprite*,          const RenderState& rs) override;
        void doRender(TextField*,       const RenderState& rs) override;
        void doRender(ColorRectSprite*, const RenderState& rs) override;
        void doRender(ProgressBar*,     const RenderState& rs) override;


    protected:
    };

}