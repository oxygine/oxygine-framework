#include "ImageDataOperations.h"
#include <string.h>

namespace oxygine
{
	namespace operations
	{
		bool check(const ImageData &src, const ImageData &dest)
		{
            OX_ASSERT(dest.w == src.w);
            OX_ASSERT(dest.h == src.h);
            OX_ASSERT(src.data);
            OX_ASSERT(dest.data);
            OX_ASSERT(src.pitch);
            OX_ASSERT(dest.pitch);
            OX_ASSERT(src.bytespp);
            OX_ASSERT(dest.bytespp);
			if (dest.w != src.w ||
				dest.h != src.h ||
				!src.data || 
				!dest.data ||
				!src.pitch ||
				!dest.pitch ||
				!src.bytespp||
				!dest.bytespp)
				return false;

			return true;
		}

		void copy(const ImageData &src, ImageData &dest)
		{
			if (!check(src, dest))
				return;

            OX_ASSERT(src.format == dest.format);

			int bppPitch = src.w * src.bytespp;

			if (src.pitch == dest.pitch && bppPitch == dest.pitch)
				memcpy(dest.data, src.data, bppPitch * src.h);
			else
			{
				const unsigned char *srcLine = src.data;
				unsigned char *destLine = dest.data;

				for (int h = 0; h < src.h; h++)
				{
					memcpy(destLine, srcLine, bppPitch);
					srcLine += src.pitch;
					destLine += dest.pitch;
				}
			}
		}

		void blit(const ImageData &src, ImageData &dest)
		{
			if (src.format == dest.format)
			{
				copy(src, dest);
				return;
			}
			
			op_blit op;
			applyOperation(op, src, dest);
		}
        
    	void blitPremultiply(const ImageData &src, ImageData &dest)
        {
            op_premultipliedAlpha op;
            applyOperation(op, src, dest);
        }

		void flipY(const ImageData &src, ImageData &dest)
		{
			if (!check(src, dest))
				return;

			const unsigned char *srcLine = src.data;
			unsigned char *destLine = dest.data + dest.pitch * dest.h - dest.pitch;

			int bppPitch = src.w * src.bytespp;

			for (int h = 0; h < src.h; h++)
			{
				memcpy(destLine, srcLine, bppPitch);
				srcLine += src.pitch;
				destLine -= dest.pitch;
			}
		}
	}
}
