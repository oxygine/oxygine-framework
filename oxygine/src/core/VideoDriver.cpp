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

	void VideoDriverNull::begin(const Rect &viewport, const Color *clearColor)
	{

	}

	void VideoDriverNull::getStats(Stats &s) const
	{
		s.batches = 0;
		s.triangles = 0;
	}

	void VideoDriverNull::getViewport(Rect &r) const
	{
		
	}
	
	bool VideoDriverNull::getScissorRect(Rect &) const
	{
		return false;
	}

	const VertexDeclaration*	VideoDriverNull::getVertexDeclaration(bvertex_format) const
	{
		OX_ASSERT(0);
		return 0;
	}

	void VideoDriverNull::setScissorRect(const Rect *)
	{

	}

	void VideoDriverNull::setDefaultSettings()
	{

	}
	void VideoDriverNull::setRenderTarget(spNativeTexture)
	{

	}
	void VideoDriverNull::setShaderProgram(ShaderProgram*)
	{

	}
	void VideoDriverNull::setTexture(int sampler, spNativeTexture)
	{

	}
}