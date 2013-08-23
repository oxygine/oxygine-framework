#pragma once
#include "NativeTexture.h"
#include "AS3/AS3.h"
#include <Flash++.h>

using namespace AS3::ui;

namespace oxygine
{
	class NativeTextureStage3D: public NativeTextureNull
	{
	public:
		 NativeTextureStage3D();
		 ~NativeTextureStage3D();

		 void init(nativeTextureHandle, int w, int h, TextureFormat tf);
		 void init(const ImageData &src, bool sysMemCopy);

		 nativeTextureHandle getHandle() const;
		 int getWidth() const;
		 int getHeight() const;
		 TextureFormat getFormat() const;

		 flash::display3D::textures::Texture getTexture() {return _handle;}

	private:
		bool _valid;
		flash::display3D::textures::Texture _handle;
		int _width;
		int _height;

		TextureFormat _format;
	};
}