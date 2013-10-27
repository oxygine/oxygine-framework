#include "NativeTextureStage3D.h"
#include "VideoDriverStage3D.h"

namespace oxygine
{
	NativeTextureStage3D::NativeTextureStage3D():_width(0), _height(0), _format(TF_UNDEFINED), _valid(false)
	{

	}

	NativeTextureStage3D::~NativeTextureStage3D()
	{
		
	}

	void NativeTextureStage3D::init(const ImageData &src, bool sysMemCopy)	
	{
		VideoDriverStage3D *vd = (VideoDriverStage3D*)IVideoDriver::instance;

		_valid = true;

		_handle = vd->getContext()->createTexture(src.w, src.h, flash::display3D::Context3DTextureFormat::BGRA, false);
		_handle->uploadFromByteArray(internal::get_ram(), (int)src.data, 0, 0);

		_width = src.w;
		_height = src.h;
		_format = src.format;
		/*
		unsigned int id = createTexture();

		glPixel p = SurfaceFormat2GL(src.format);
		if (src.format == TF_PVRTC4)
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, p.format, src.w, src.h, 0, src.h * src.pitch, src.data);
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, p.format, src.w, src.h, 0, p.format, p.type, src.data);			
		}

		OX_ASSERT(sysMemCopy == false);

		init((nativeTextureHandle)id, src.w, src.h, src.format);
		*/
	}


	void NativeTextureStage3D::init(nativeTextureHandle, int w, int h, TextureFormat tf)
	{
		_width = w;
		_height = h;

		_format = tf;
	}

	nativeTextureHandle NativeTextureStage3D::getHandle() const
	{
		if (_valid)
			return (void*)1;
		return 0;
	}

	int NativeTextureStage3D::getWidth() const
	{
		return _width;
	}
	
	int NativeTextureStage3D::getHeight() const
	{
		return _height;
	}

	TextureFormat NativeTextureStage3D::getFormat() const
	{
		return _format;
	}
}