#include "NativeTexture.h"
#include "MemoryTexture.h"
#include "ImageDataOperations.h"
#include "file.h"
#include "log.h"
#include <stdio.h>

namespace oxygine
{
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

		const ObjectBase::__createdObjects &obj = ObjectBase::__getCreatedObjects();
		for (ObjectBase::__createdObjects::const_iterator i = obj.begin(); i != obj.end(); ++i)
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

	void NativeTextureNull::setLinearFilter(bool enable)
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
}