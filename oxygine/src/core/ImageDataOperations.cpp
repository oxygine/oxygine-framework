#include "ImageDataOperations.h"
namespace oxygine
{
	namespace operations
	{
		bool check(const ImageData &src, const ImageData &dest)
		{
			assert(dest.w == src.w);
			assert(dest.h == src.h);
			assert(src.data);
			assert(dest.data);
			assert(src.pitch);
			assert(dest.pitch);
			assert(src.bytespp);
			assert(dest.bytespp);
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

			assert(src.format == dest.format);

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