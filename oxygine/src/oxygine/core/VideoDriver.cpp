#include "VideoDriver.h"
#include "NativeTexture.h"
#include "../math/Color.h"
#include "ShaderProgram.h"


namespace oxygine
{
    IVideoDriver* IVideoDriver::instance = 0;

    IVideoDriver::~IVideoDriver()
    {

    }

    void IVideoDriver::setUniform(const char* id, const Matrix& v)
    {
        setUniform(id, &v, 1);
    }

    void IVideoDriver::setUniform(const char* id, const Vector2& v)
    {
        setUniform(id, &v, 1);
    }

    void IVideoDriver::setUniform(const char* id, const Vector3& v)
    {
        setUniform(id, &v, 1);
    }

    void IVideoDriver::setUniform(const char* id, const Vector4& v)
    {
        setUniform(id, &v, 1);
    }

    void IVideoDriver::_debugAddPrimitives(PRIMITIVE_TYPE pt, int primitives)
    {
#if OXYGINE_TRACE_VIDEO_STATS
        _stats.elements[pt] += primitives;
        _stats.batches += 1;
#endif

    }

    IVideoDriver::Stats IVideoDriver::_stats;

    spNativeTexture VideoDriverNull::createTexture()
    {
        //return new NativeTextureNull;
        return 0;
    }

    void VideoDriverNull::begin(const Rect& viewport, const Color* clearColor)
    {

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