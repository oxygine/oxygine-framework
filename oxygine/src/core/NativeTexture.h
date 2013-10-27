#pragma once
#include "oxygine_include.h"
#include "closure/closure.h"
#include "core/Restorable.h"
#include "ox_debug.h"
#include "Texture.h"

namespace oxygine
{
	void init_ext();

	DECLARE_SMART(NativeTexture, spNativeTexture);
	DECLARE_SMART(RenderTexture, spRenderTexture);

	typedef void* nativeTextureHandle;

	
	class NativeTexture: public Texture, public Restorable
	{
	public:
		NativeTexture():_vram(0){}
		virtual void init(nativeTextureHandle, int w, int h, TextureFormat tf) = 0;
		virtual void init(int w, int h, TextureFormat tf, bool renderTarget = false) = 0;
		virtual void init(const ImageData &src, bool sysMemCopy) = 0;
		//virtual void release() = 0;

		virtual ImageData lock(lock_flags, const Rect *src) = 0;
		virtual void unlock() = 0;
		virtual void swap(NativeTexture *) = 0;

		virtual void setLinearFiltration(bool enable) = 0;
		virtual void updateRegion(int x, int y, const ImageData &data) = 0;
		virtual void apply(const Rect *rect = 0) = 0;

		/**returns handle (ptr) to HW texture ID*/
		virtual nativeTextureHandle getHandle() const = 0;
		
		unsigned int getSizeVRAM() const {return _vram;}
		//debug
		static void dumpCreatedTextures();
		static std::vector<spNativeTexture> getCreatedTextures();
		/**debug counter of created textures*/
		static volatile int created;

	protected:
		unsigned int _vram;
	};

	class NativeTextureNull: public NativeTexture
	{
	public:
		void init(nativeTextureHandle, int w, int h, TextureFormat tf);
		void init(int w, int h, TextureFormat tf, bool renderTarget = false);
		void init(const ImageData &src, bool sysMemCopy);
		void release();

		ImageData lock(lock_flags, const Rect *src);
		void unlock();
		void swap(NativeTexture *);

		void setLinearFiltration(bool enable);
		void updateRegion(int x, int y, const ImageData &data);
		void apply(const Rect *rect = 0);

		/**returns handle (ptr) to HW texture ID*/
		nativeTextureHandle getHandle() const;
		int getWidth() const;
		int getHeight() const;
		TextureFormat getFormat() const;
	};


	/**Returns View matrix where Left Top corner is (0,0), and right bottom is (w,h)*/
	Matrix makeViewMatrix(int w, int h, bool flipU = false);
}