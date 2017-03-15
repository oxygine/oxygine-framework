#pragma once
#include "../../oxygine-include.h"
#include "../VideoDriver.h"
#include "VertexDeclarationGL.h"
#include "oxgl.h"

namespace oxygine
{
    class VertexDeclarationGL;
    DECLARE_SMART(NativeTextureGLES, spNativeTextureGLES);

    /**intermediate gl driver with some shared gl code*/
    class VideoDriverGL: public IVideoDriver
    {
    public:
        VideoDriverGL();


        void    getViewport(Rect& r) const;
        bool    getScissorRect(Rect&) const;
        spNativeTexture getRenderTarget() const;
        const VertexDeclarationGL*  getVertexDeclaration(bvertex_format) const;

        void setScissorRect(const Rect*);
        void setRenderTarget(spNativeTexture rt);

        void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest);
        void setState(STATE, unsigned int value);


    protected:
        unsigned int getPT(IVideoDriver::PRIMITIVE_TYPE pt);
        unsigned int getBT(IVideoDriver::BLEND_TYPE pt);

        void _begin(const Rect& viewport, const Color* clearColor);
        spNativeTextureGLES _rt;

        mutable VertexDeclarations<VertexDeclarationGL> _vdeclarations;
    };
}