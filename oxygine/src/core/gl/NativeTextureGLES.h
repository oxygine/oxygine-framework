#pragma once
#include "oxygine_include.h"
#include "../NativeTexture.h"

namespace oxygine
{
	class NativeTextureGLES : public NativeTexture
	{
	public:
		~NativeTextureGLES();

		void init(nativeTextureHandle id, int w, int h, TextureFormat tf);
		void init(int w, int h, TextureFormat tf, bool renderTarget);
		void init(const ImageData &src, bool sysMemCopy);
		void release();
		void swap(NativeTexture *) ;

		nativeTextureHandle	getHandle() const;
		int					getWidth() const;
		int					getHeight() const;
		TextureFormat		getFormat() const;
		unsigned int		getFboID() const;

		ImageData lock(lock_flags, const Rect *src);
		void unlock();

		void setLinearFiltration(bool enable);

		void updateRegion(int x, int y, const ImageData &data);
		void apply(const Rect *rect = 0);

	protected:
		void* _getRestorableObject(){return this;}
		friend class VideoDriverGLES11;
		friend class VideoDriverGLES20;
		NativeTextureGLES();

		unsigned int _id;
		unsigned int _fbo;

		TextureFormat _format;
		int _width;
		int _height;
		ImageData _im;

		std::vector<unsigned char> _data;
		int _lockFlags;
		Rect _lockRect;
	};
}