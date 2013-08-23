#pragma once
#include "ref_counter.h"
#include "Object.h"
#include <vector>
#include "math/Rect.h"
#include "ImageData.h"

namespace oxygine
{
	DECLARE_SMART(Texture, spTexture);

	inline unsigned int nextPOT(unsigned int v)
	{
		v = v - 1;
		v = v | (v >> 1);
		v = v | (v >> 2);
		v = v | (v >> 4);
		v = v | (v >> 8);
		v = v | (v >>16);
		return v + 1;
	}

	class Texture : public Object
	{
	public:		
		enum 
		{
			lock_read = 0x1,
			lock_write = 0x2,
			lock_write_on_apply = 0x4,
		};

		typedef int lock_flags;
				
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
		virtual TextureFormat getFormat() const = 0;		
		virtual unsigned int getSizeVRAM() const = 0;

		virtual ImageData lock(lock_flags, const Rect *src) = 0;
		virtual void unlock() = 0;
		virtual void updateRegion(int x, int y, const ImageData &data) = 0;
		virtual void apply(const Rect *rect = 0) = 0;
	};
}