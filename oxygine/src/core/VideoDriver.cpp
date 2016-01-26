#include "VideoDriver.h"
#include "NativeTexture.h"
#include "math/Color.h"
#include "ShaderProgram.h"


namespace oxygine
{
    IVideoDriver* IVideoDriver::instance = 0;

    spNativeTexture VideoDriverNull::createTexture()
    {
        //return new NativeTextureNull;
        return 0;
    }

    void VideoDriverNull::begin(const Rect& viewport, const Color* clearColor)
    {

    }

    void VideoDriverNull::getStats(Stats& s) const
    {
        s.batches = 0;
        s.triangles = 0;
    }

    void VideoDriverNull::getViewport(Rect& r) const
    {

    }

    bool VideoDriverNull::getScissorRect(Rect&) const
    {
        return false;
    }

    spNativeTexture VideoDriverNull::getRenderTarget() const
    {
        return _rt;
    }

    const VertexDeclaration*    VideoDriverNull::getVertexDeclaration(bvertex_format bf) const
    {
        return IVideoDriver::instance->getVertexDeclaration(bf);
    }

    void VideoDriverNull::setScissorRect(const Rect*)
    {

    }

    void VideoDriverNull::setDefaultSettings()
    {

    }
    void VideoDriverNull::setRenderTarget(spNativeTexture rt)
    {
        _rt = rt;
    }
    void VideoDriverNull::setShaderProgram(ShaderProgram*)
    {

    }
    void VideoDriverNull::setTexture(int sampler, spNativeTexture)
    {

    }
}