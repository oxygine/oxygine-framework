#pragma once
#include "oxygine_include.h"

namespace oxygine
{
	const unsigned char lookupTable4to8[] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
	const unsigned char lookupTable5to8[] = {0, 8, 16, 24, 32, 41, 49, 57, 65, 74, 82, 90, 98, 106, 115, 123, 131, 139, 148, 156, 164, 172, 180, 189, 197, 205, 213, 222, 230, 238, 246, 255};
	const unsigned char lookupTable6to8[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 238, 242, 246, 250, 255};

	struct Pixel
	{
		union 
		{
			struct
			{
				unsigned char bytes[4];
			};

			struct
			{
				unsigned char r, g, b, a;
			};			

			unsigned int rgba;
		};
	};

	inline Pixel initPixel(unsigned int rgba)
	{
		Pixel p;
		p.rgba = rgba;
		return p;
	}


	class PixelA8
	{
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			p.r = 255;
			p.g = 255;
			p.b = 255;
			p.a = data[0];
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{			
			*data = p.a;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned char *)dst) = *((unsigned char*)src);
		}
	};

	class PixelL8
	{
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned char color = *data;
			p.r = color;
			p.g = color;
			p.b = color;
			p.a = 255;
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{			
			*data = (p.r + p.g + p.b)/3;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*dst = *src;
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return 255;
		}
	};

	class PixelA8L8
	{
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned char color = *data;
			p.r = color;
			p.g = color;
			p.b = color;
			p.a = 255;
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{			
			*data = (p.r + p.g + p.b)/3;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*dst = *src;
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return 255;
		}
	};

	class PixelR5G6B5
	{
		/*
		in memory: BBBBB_GGGGGG_RRRRR
		in dword:  RRRRR_GGGGGG_BBBBB
		*/
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned short color = *((unsigned short *)data);
			p.r = lookupTable5to8[(color & 0xF800) >> 11];
			p.g = lookupTable6to8[(color & 0x7E0) >> 5];
			p.b = lookupTable5to8[(color & 0x1F)];
			p.a = 255;
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			unsigned short *pshort = (unsigned short *)data;
			*pshort = ((p.r >> 3) << 11) | ((p.g >> 2) << 5) | (p.b >> 3);
		}
		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned short *)dst) = *((unsigned short*)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return 255;
		}
	};

	

	/*
	class PixelA4R4G4B4
	{
		
		//in memory: BBBB_GGGG_RRRR_AAAA
		//in word: AAAA_RRRR_GGGG_BBBB
		
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned short color = *((unsigned short *)data);

			p.r = table4to8[((color & 0xF00) >> 8)];
			p.g = table4to8[((color & 0xF0))];
			p.b = table4to8[((color & 0xF) << 4)];
			p.a = table4to8[((color & 0xF000) >> 12)];
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			unsigned short *pshort = (unsigned short *)data;
			*pshort = ((p.a>>4) << 12) | ((p.r>>4) << 8) | ((p.g>>4) << 4) | ((p.b>>4));
		}
		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned short *)dst) = *((const unsigned short*)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return table4to8[(alpha >> 4)];
		}
	};
	*/

	class PixelR4G4B4A4
	{
		/*
		in memory: AAAA_BBBB_GGGG_RRRR
		in word: RRRR_GGGG_BBBB_AAAA
		*/
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned short color = *((unsigned short *)data);

			p.r = lookupTable4to8[(color & 0xF000) >> 12];
			p.g = lookupTable4to8[(color & 0xF00) >> 8];
			p.b = lookupTable4to8[(color & 0xF0) >> 4];
			p.a = lookupTable4to8[(color & 0xF)];

			/*
			p.r = ((color & 0xF000) >> 8);
			p.g = ((color & 0xF00) >> 4);
			p.b = ((color & 0xF0));
			p.a = ((color & 0xF) << 4);
			*/
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			unsigned short *pshort = (unsigned short *)data;
			*pshort = ((p.r >> 4) << 12) | ((p.g>>4) << 8) | ((p.b>>4) << 4) | ((p.a>>4));
		}
		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned short *)dst) = *((const unsigned short*)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return lookupTable4to8[alpha >> 4];
		}
	};

	/*
	class PixelA1R5G5B5
	{
		
		//in memory: BBBBB_GGGGG_RRRRR_A
		//in dword: A_RRRRR_GGGGG_BBBBB
		
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned short color = *((unsigned short *)data);

			p.r = lookupTable5to8[(color & 0x7C00) >> 7];
			p.g = lookupTable5to8[(color & 0x3E0) >> 2];
			p.b = lookupTable5to8[(color & 0x1F) << 3];
			p.a = ((color & 0x8000) >> 15) * 255;
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			unsigned short *pshort = (unsigned short *)data;
			*pshort = ((p.a>>7) << 15) | ((p.r>>3) << 10) | ((p.g>>3) << 5) | ((p.b>>3));
		}
		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned short *)dst) = *((const unsigned short*)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return (alpha >> 7) * 255;
		}
	};
	*/

	class PixelR5G5B5A1
	{
		//in memory: A_BBBBB_GGGGG_RRRRR
		//in dword: RRRRR_GGGGG_BBBBB_A
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			unsigned short color = *((unsigned short *)data);

			p.r = lookupTable5to8[(color & 0xF800) >> 11];
			p.g = lookupTable5to8[(color & 0x7C0) >> 6];
			p.b = lookupTable5to8[(color & 0x3E) >> 1];
			p.a = (color & 0x01) * 255;
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			unsigned short *pshort = (unsigned short *)data;
			*pshort = ((p.a>>7)) | ((p.r>>3) << 11) | ((p.g>>3) << 6) | ((p.b>>3) << 1);
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned short *)dst) = *((const unsigned short*)src);
		}			
		unsigned char snap_a(unsigned char alpha) const
		{
			return (alpha >> 7) * 255;
		}
	};

	class PixelA8B8G8R8
	{
		/*
		in memory: A8 B8 G8 R8
		*/
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			p.r = data[3];
			p.g = data[2];
			p.b = data[1];
			p.a = data[0];

		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			data[0] = p.a;
			data[1] = p.b;
			data[2] = p.g;
			data[3] = p.r;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned int *)dst) = *((unsigned int *)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return alpha;
		}
	};



	class PixelA8R8G8B8
	{
		/*
		in memory: B8 G8 R8 A8
		in dword: A8 R8 G8 B8
		*/
	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			p.r = data[2];
			p.g = data[1];
			p.b = data[0];
			p.a = data[3];

		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			data[0] = p.b;
			data[1] = p.g;
			data[2] = p.r;
			data[3] = p.a;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned int *)dst) = *((unsigned int *)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return alpha;
		}
	};

	class PixelR8G8B8A8
	{
		/*
		in memory: R8 G8 B8 A8
		in dword: A8 B8 G8 R8
		*/

	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			p.r = data[0];
			p.g = data[1];
			p.b = data[2];
			p.a = data[3];
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			data[0] = p.r;
			data[1] = p.g;
			data[2] = p.b;
			data[3] = p.a;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned int *)dst) = *((unsigned int *)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return alpha;
		}
	};

	class PixelB8G8R8A8
	{
		/*
		in memory: B8 G8 R8 A8
		in dword: A8 R8 G8 B8
		*/

	public:
		void getPixel(const unsigned char *data, Pixel &p) const
		{
			p.r = data[2];
			p.g = data[1];
			p.b = data[0];
			p.a = data[3];
		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			data[2] = p.r;
			data[1] = p.g;
			data[0] = p.b;
			data[3] = p.a;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned int *)dst) = *((unsigned int *)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return alpha;
		}
	};

	class PixelX8R8G8B8 : public PixelA8R8G8B8{};
	class PixelR8G8B8X8 : public PixelR8G8B8A8{};

	class PixelR8G8B8
	{
		/*
		in memory: R8 G8 B8
		in dword: B8 G8 R8
		*/
	public:
		PixelR8G8B8(unsigned char alpha = 255):_alpha(alpha){}

		void getPixel(const unsigned char *data, Pixel &p) const
		{
			p.r = data[0];
			p.g = data[1];
			p.b = data[2];
			p.a = _alpha;

		}

		void setPixel(unsigned char *data, const Pixel &p)
		{
			data[0] = p.r;
			data[1] = p.g;
			data[2] = p.b;
		}

		void copy(const unsigned char *src, unsigned char *dst)
		{
			*((unsigned short *)dst) = *((const unsigned short*)src);
		}

		unsigned char snap_a(unsigned char alpha) const
		{
			return _alpha;
		}

	private:
		unsigned char _alpha;
	};
}