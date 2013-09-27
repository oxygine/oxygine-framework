#include <stdio.h>
#include "oxygine.h"
#include "NativeTexture.h"
#include "MemoryTexture.h"
#include "ImageDataOperations.h"
#include "file.h"
#include "log.h"

/*
#define DEC_FUNC(proc, name) proc name = 0;

#define LOAD_FUNC(proc, name) name = (proc)eglGetProcAddress(#name);

DEC_FUNC(PFNGLGENFRAMEBUFFERSOESPROC, glGenFramebuffersOES);
DEC_FUNC(PFNGLBINDFRAMEBUFFEROESPROC, glBindFramebufferOES);
DEC_FUNC(PFNGLFRAMEBUFFERTEXTURE2DOESPROC, glFramebufferTexture2DOES);
DEC_FUNC(PFNGLCHECKFRAMEBUFFERSTATUSOESPROC, glCheckFramebufferStatusOES);
DEC_FUNC(PFNGLDELETEFRAMEBUFFERSOESPROC, glDeleteFramebuffersOES);
*/

namespace oxygine
{
	void init_ext()
	{

/*		glGenFramebuffersOES = (PFNGLGENFRAMEBUFFERSOESPROC)eglGetProcAddress("glGenFramebuffersOES");
		LOAD_FUNC(PFNGLGENFRAMEBUFFERSOESPROC, glGenFramebuffersOES);
		LOAD_FUNC(PFNGLBINDFRAMEBUFFEROESPROC, glBindFramebufferOES);
		LOAD_FUNC(PFNGLFRAMEBUFFERTEXTURE2DOESPROC, glFramebufferTexture2DOES);
		LOAD_FUNC(PFNGLCHECKFRAMEBUFFERSTATUSOESPROC, glCheckFramebufferStatusOES);
		LOAD_FUNC(PFNGLDELETEFRAMEBUFFERSOESPROC, glDeleteFramebuffersOES);
	*/};

	

	volatile int NativeTexture::created = 0;

	void NativeTexture::dumpCreatedTextures()
	{
		log::messageln("created native textures dump:");
		std::vector<spNativeTexture> t = getCreatedTextures();

		int n = 0;
		for (std::vector<spNativeTexture>::iterator i = t.begin(); i != t.end(); ++i)
		{
			spNativeTexture nt = *i;

			log::message("%d)", n);
			++n;
			nt->dumpObject();
		}
		log::messageln("----");
	}

	std::vector<spNativeTexture> NativeTexture::getCreatedTextures()
	{
		std::vector<spNativeTexture> t;

		const ObjectBase::createdObjects &obj = ObjectBase::getCreatedObjects();
		for (ObjectBase::createdObjects::const_iterator i = obj.begin(); i != obj.end(); ++i)
		{
			ObjectBase *ob = *i;
			NativeTexture *nt = dynamic_cast<NativeTexture *>(ob);
			if (!nt)
				continue;
			if (!nt->getHandle())
				continue;

			t.push_back(nt);
		}
		return t;
	}


	void NativeTextureNull::init(nativeTextureHandle, int w, int h, TextureFormat tf)
	{

	}

	void NativeTextureNull::init(int w, int h, TextureFormat tf, bool renderTarget)
	{

	}

	void NativeTextureNull::init(const ImageData &src, bool sysMemCopy)
	{

	}

	void NativeTextureNull::release()
	{

	}

	ImageData NativeTextureNull::lock(lock_flags, const Rect *src)
	{
		return ImageData();
	}

	void NativeTextureNull::unlock()
	{

	}

	void NativeTextureNull::swap(NativeTexture *)
	{

	}

	void NativeTextureNull::setLinearFiltration(bool enable)
	{

	}

	void NativeTextureNull::updateRegion(int x, int y, const ImageData &data)
	{

	}

	void NativeTextureNull::apply(const Rect *rect)
	{

	}

	/**returns handle (ptr) to HW texture ID*/
	nativeTextureHandle NativeTextureNull::getHandle() const
	{
		return 0;
	}

	int NativeTextureNull::getWidth() const
	{
		return 0;
	}

	int NativeTextureNull::getHeight() const
	{
		return 0;
	}
	TextureFormat NativeTextureNull::getFormat() const
	{
		return TF_UNDEFINED;
	}

	Matrix makeViewMatrix(int w, int h, bool flipU)
	{
		//printf("s1\n");
		Matrix view, scale, tr;
		float offset = 0.5f;

		offset = 0;

		if (flipU)
		{
			//printf("s2\n");
			Matrix::translation(tr, Vector3(-(float)w / 2.0f - offset, -(float)h / 2.0f + offset, 0.0f));
			Matrix::scaling(scale, Vector3(1.0f, 1.0f, 1.0f));

			view = scale * tr;
		}
		else
		{
			/*
			Matrix::translation(tr, Vector3((float)w / 2.0f - offset, (float)h / 2.0f + offset, 0.0f));			
			Matrix::scaling(scale, Vector3(-1.0f, 1.0f, 1.0f));
			Matrix rot;
			Matrix::rotationZ(rot, M_PI/2);
			view = scale * rot * tr;
			*/


			Matrix::translation(tr, Vector3(-(float)w / 2.0f - offset, (float)h / 2.0f + offset, 0.0f));
			Matrix::scaling(scale, Vector3(1.0f, -1.0f, 1.0f));


			//Matrix::translation(tr, Vector3(-(float)w / 2.0f - offset, -(float)h / 2.0f + offset, 0.0f));
			//Matrix::scaling(scale, Vector3(1.0f, -1.0f, 1.0f));


			view = scale * tr;
		}

		return view;
	}
}