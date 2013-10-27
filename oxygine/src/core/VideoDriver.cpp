#include "VideoDriver.h"
#include "NativeTexture.h"
#include "math/Color.h"


namespace oxygine
{
	IVideoDriver* IVideoDriver::instance = 0;

	batch::batch():blend(blend_premultiplied_alpha), program(0), basePremultiplied(false), vdecl(0), maskChannelR(false){}
	batch::~batch()
	{
	}


	typedef std::vector<unsigned char> indices8;
	typedef std::vector<unsigned short> indices16;

	indices8 initIndices8()
	{
		indices8 ind;
		for (int t = 0; t < 60; t += 1)
		{
			int i = t * 4;
			ind.push_back(i + 0);
			ind.push_back(i + 1);
			ind.push_back(i + 2);

			ind.push_back(i + 2);
			ind.push_back(i + 1);
			ind.push_back(i + 3);
		}

		return ind;
	}

	indices16 initIndices16()
	{
		indices16 ind;
		ind.reserve(12000 * 6);
		for (int t = 0; t < 12000; t += 1)
		{
			int i = t * 4;
			ind.push_back(i + 0);
			ind.push_back(i + 1);
			ind.push_back(i + 2);

			ind.push_back(i + 2);
			ind.push_back(i + 1);
			ind.push_back(i + 3);
		}

		return ind;
	}

	indices8 _indices8 = initIndices8();
	indices16 _indices16 = initIndices16();



	spNativeTexture VideoDriverNull::createTexture()
	{
		//return new NativeTextureNull;
		return 0;
	}

	void VideoDriverNull::begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *clearColor)
	{

	}

	void VideoDriverNull::getViewport(Rect &r) const
	{
		
	}

	int VideoDriverNull::getMaxVertices() const
	{
		return 255;
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
	
	void VideoDriverNull::drawBatch(const batch &b)
	{

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
}