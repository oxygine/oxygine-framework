#include "MemoryTexture.h"
#include "math/Rect.h"
#include "core/ImageDataOperations.h"
#include "core/file.h"
#include "core/log.h"

extern "C"
{
    #include "png.h"
	#include "jpeglib.h"
};

//#define  LOGD(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)  

namespace oxygine
{
	enum ImageType
	{
		IT_UNKNOWN,
		IT_PNG,
		IT_PKM,
		IT_PVR,
		IT_JPEG
	};
	class CCImageHelper
	{
	public:
		CCImageHelper();
		~CCImageHelper();
		// dummy funcs to help libjpeg
		static void JPEGInitSource(j_decompress_ptr cinfo)
		{
		}

		static boolean JPEGFillInputBuffer(j_decompress_ptr cinfo)
		{
			return 0;
		}

		static void JPEGSkipInputData(j_decompress_ptr cinfo, long num_bytes)
		{
			cinfo->src->next_input_byte += num_bytes;
			cinfo->src->bytes_in_buffer -= num_bytes;
		}

		static void JPEGTermSource(j_decompress_ptr cinfo)
		{
		}
	};


	ImageType getImageType(const void *Data, size_t size)
	{
		unsigned char *data = (unsigned char *)Data;
		const unsigned int &dt = *((unsigned int*)data);
		if (data[0] == 0x89 && data[1] == 'P' && data[2] == 'N' && data[3] == 'G' )
			return IT_PNG;
		if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF)
			return IT_JPEG;
		if (data[0] == 'P' && data[1] == 'K' && data[2] == 'M')
			return IT_PKM;
		if (dt == 0x03525650)
			return IT_PVR;

		return IT_UNKNOWN;
	}

	struct pkm_header
	{
		char id[3];
		short version;
		short type;
		unsigned short width;
		unsigned short height;
		unsigned short original_width;
		unsigned short original_height;
	};

	struct pvr_header
	{
		unsigned int version;
		unsigned int flags;
		unsigned int formatA;
		unsigned int formatB;
		unsigned int color_space;
		unsigned int channel_type;
		unsigned int height;
		unsigned int width;
		unsigned int depth;
		unsigned int num_surfaces;
		unsigned int num_faces;
		unsigned int num_mips;
		unsigned int meta_data_size;
	};
	
	unsigned short swapByteOrder(unsigned short us)
	{
		return (us >> 8) |	(us << 8);
	}

	bool _initWithPkmData(MemoryTexture &mt, file::buffer &bf)
	{	

		return true;
	}
	
	bool _initWithJpgData(MemoryTexture &mt, void * data, int nSize, bool premultiplied, TextureFormat format)
	{
		bool bRet = false;


		jpeg_decompress_struct cinfo;
		memset(&cinfo, 0, sizeof(cinfo));

		JSAMPARRAY buffer;      /* Output row buffer */
		int row_stride;     /* physical row width in output buffer */

		jpeg_source_mgr srcmgr;

		srcmgr.bytes_in_buffer = nSize;
		srcmgr.next_input_byte = (JOCTET*) data;
		srcmgr.init_source = CCImageHelper::JPEGInitSource;
		srcmgr.fill_input_buffer = CCImageHelper::JPEGFillInputBuffer;
		srcmgr.skip_input_data = CCImageHelper::JPEGSkipInputData;
		srcmgr.resync_to_restart = jpeg_resync_to_restart;
		srcmgr.term_source = CCImageHelper::JPEGTermSource;

		jpeg_error_mgr jerr;
		cinfo.err = jpeg_std_error(&jerr);

		jpeg_create_decompress(&cinfo);
		cinfo.src = &srcmgr;

		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		/* JSAMPLEs per row in output buffer */
		row_stride = cinfo.output_width * cinfo.output_components;

		/* Make a one-row-high sample array that will go away when done with image */
		buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

		int copy_rows  = (int)cinfo.output_height;
		int copy_width = (int)cinfo.output_width;

	   if (copy_width < 0 || copy_rows < 0)
		{
			//printf("jpeg is fully off screen\n");
			return bRet;
		}
		int startx=0;
		int starty=0;
		int bytesPerPix = 4;

		TextureFormat destFormat = format;
		if (destFormat == TF_UNDEFINED)
			destFormat = TF_R8G8B8A8;

		mt.init(copy_width, copy_rows, destFormat);
		ImageData dest = mt.lock();
		dest.h = 1;
		ImageData src(copy_width, 1, copy_width * 3, TF_R8G8B8);

		while (cinfo.output_scanline < cinfo.output_height)// count through the image
		{
			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			 * Here the array is only one element long, but you could ask for
			 * more than one scanline at a time if that's more convenient.
			 */
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);

			if (starty-- <= 0)// count down from start
			{
				if (copy_rows-- > 0)
				{
					src.data = buffer[0];
					operations::blit(src, dest);
					dest.data += dest.pitch;

					/*
					for (int xx=startx; xx < copy_width; xx++)
					{
						uint8 r = buffer[0][xx*3+0];
						uint8 b = buffer[0][xx*3+1];
						uint8 g = buffer[0][xx*3+2];

						*dst++ = r;
						*dst++ = b;
						*dst++ = g;
						*dst++ = 255;
					}

					*/
				}
			}
		}

		(void) jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		//printf("jpeg display done\n");

		bRet = true;
		return bRet;
	}


	typedef struct 
	{
		unsigned char* data;
		int size;
		int offset;
	}tImageSource;

	static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
	{		
		tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

		LOGD("png read l: %d, %d", length, isource->offset);

		if((int)(isource->offset + length) <= isource->size)
		{
			memcpy(data, isource->data+isource->offset, length);
			isource->offset += length;
		}
		else
		{
			LOGD("png error");
			png_error(png_ptr, "pngReaderCallback failed");
		}
	}



	bool _initWithPngData(MemoryTexture &mt, void * pData, int nDatalen, bool premultiplied, TextureFormat format)
	{
		LOGD("reading png...");
		bool bRet = false;
		png_byte        header[8]   = {0}; 
		png_structp     png_ptr     =   0;
		png_infop       info_ptr    = 0;

		do 
		{
			// png header len is 8 bytes
			//CC_BREAK_IF(nDatalen < 8);

			// check the data is png or not
			memcpy(header, pData, 8);
			//CC_BREAK_IF(png_sig_cmp(header, 0, 8));

			// init png_struct
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			//CC_BREAK_IF(! png_ptr);
			LOGD("reading png...1");
			// init png_info
			info_ptr = png_create_info_struct(png_ptr);
			//CC_BREAK_IF(!info_ptr);
//#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
			//CC_BREAK_IF(setjmp(png_jmpbuf(png_ptr)));
			setjmp(png_jmpbuf(png_ptr));
//#endif
			// set the read call back function
			tImageSource imageSource;
			imageSource.data    = (unsigned char*)pData;
			imageSource.size    = nDatalen;
			imageSource.offset  = 0;
			png_set_read_fn(png_ptr, &imageSource, pngReadCallback);
			LOGD("reading png...2");
			// read png
			// PNG_TRANSFORM_EXPAND: perform set_expand()
			// PNG_TRANSFORM_PACKING: expand 1, 2 and 4-bit samples to bytes
			// PNG_TRANSFORM_STRIP_16: strip 16-bit samples to 8 bits
			// PNG_TRANSFORM_GRAY_TO_RGB: expand grayscale samples to RGB (or GA to RGBA)
			png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_PACKING 
				| PNG_TRANSFORM_STRIP_16 /*| PNG_TRANSFORM_GRAY_TO_RGB*/, 0);

			LOGD("reading png...2a");
			int         color_type  = 0;
			png_uint_32 nWidth = 0;
			png_uint_32 nHeight = 0;
			int         nBitsPerComponent = 0;
			png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nBitsPerComponent, &color_type, 0, 0, 0);

			LOGD("reading png...3");
			
			TextureFormat srcFormat = TF_UNDEFINED;
			if (color_type == PNG_COLOR_TYPE_GRAY)
				srcFormat = TF_L8;
			if (color_type == PNG_COLOR_MASK_ALPHA)
			{
				srcFormat = TF_L8;
				log::warning("unsupported png file pixel format");
			}

			if (color_type == PNG_COLOR_TYPE_RGB)
				srcFormat = TF_R8G8B8;
			if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
				srcFormat = TF_R8G8B8A8;

			TextureFormat destFormat = format;
			if (destFormat == TF_UNDEFINED)
				destFormat = srcFormat;

			if (destFormat == TF_L8 || destFormat == TF_A8L8 || destFormat == TF_R8G8B8)//don't support it
			{
				destFormat = TF_R8G8B8A8;
			}

			LOGD("reading png...4, %d %d", nWidth, nHeight);
			mt.init(nWidth, nHeight, destFormat);

			ImageData dest = mt.lock();


			png_bytep * rowPointers = png_get_rows(png_ptr, info_ptr);

			LOGD("reading png...5");
			ImageData src(nWidth, 1, png_get_rowbytes(png_ptr, info_ptr), srcFormat,  0);
			if (color_type == 4)
			{
				//src.data += 1;
				//src.bytespp = 2;
			}


			dest.h = 1;


			if (premultiplied)
			{
				operations::op_premultipliedAlpha op;
				for (unsigned int j = 0; j < nHeight; ++j)
				{
					src.data = rowPointers[j];
					operations::applyOperation(op, src, dest);

					dest.data += dest.pitch;
				}
			}
			else
			{
				operations::op_blit op;
				for (unsigned int j = 0; j < nHeight; ++j)
				{
					src.data = rowPointers[j];
					operations::applyOperation(op, src, dest);

					dest.data += dest.pitch;
				}
			}
			
			LOGD("reading png...6");

			bRet        = true;
		} while (0);

		//CC_SAFE_DELETE_ARRAY(pImateData);

		if (png_ptr)
		{
			png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
		}
		LOGD("reading png...7");
		return bRet;
	}



	MemoryTexture::MemoryTexture():_offset(0)
	{

	}

	MemoryTexture::~MemoryTexture()
	{

	}	

	void MemoryTexture::cleanup()
	{
		_buffer.resize(0);
		_image = ImageData();
	}

	void MemoryTexture::convert(MemoryTexture &dest, TextureFormat format)
	{
		dest.init(getWidth(), getHeight(), format);
		ImageData src = lock();
		ImageData dst = dest.lock();

		operations::blit(src, dst);
	}
	
	void MemoryTexture::fill_zero()
	{
		if (_buffer.empty())
			return;
		memset(&_buffer.front(), 0, _buffer.size());
	}

	void MemoryTexture::init(file::buffer &buffer, bool premultiplied, TextureFormat format)
	{
		cleanup();


		if (buffer.getSize())
		{
			ImageType type = getImageType(buffer.getData(), buffer.getSize());
			switch(type)
			{
			case IT_PNG:
				_initWithPngData(*this, (void*)buffer.getData(), buffer.getSize(), premultiplied, format);
				break;
			case IT_JPEG:
				_initWithJpgData(*this, (void*)buffer.getData(), buffer.getSize(), premultiplied, format);
				break;
			case IT_PKM:
				{
					const pkm_header *header = (const pkm_header *)buffer.getData();
					_image.w = swapByteOrder(header->width);
					_image.h = swapByteOrder(header->height);
					int or_w = swapByteOrder(header->original_width);
					int or_h = swapByteOrder(header->original_height);
					OX_ASSERT(_image.w == or_w && _image.h == or_h);

					_image.bytespp = 0;
					_image.format = TF_ETC1;
					_offset = sizeof(pkm_header);
					_image.pitch = (buffer.getSize() - _offset) / _image.h;
					_buffer.swap(buffer.data);
				}
				break;
			case IT_PVR:
				{
					const pvr_header *header = (const pvr_header *)buffer.getData();
					_image.w = header->width;
					_image.h = header->height;
					_image.bytespp = 0;

					TextureFormat tf = TF_UNDEFINED;
					switch(header->formatA)
					{
					case 0:
						tf = TF_PVRTC_2RGB;
						break;
					case 1:
						tf = TF_PVRTC_2RGBA;
						break;
					case 2:
						tf = TF_PVRTC_4RGB;
						break;
					case 3:
						tf = TF_PVRTC_4RGBA;
						break;
					case 4:
						tf = TF_PVRTCII_2;
						break;
					case 5:
						tf = TF_PVRTCII_4;
						break;
					case 6:
						tf = TF_ETC1;
						break;
					}
					_image.format = tf;
					_offset = sizeof(pvr_header) + header->meta_data_size;
					_image.pitch = (buffer.getSize() - _offset) / _image.h;
					_buffer.swap(buffer.data);
				}
				break;
			default:
				log::warning("MemoryTexture. can't unpack data %s");
				break;
			}			
		}
	}

	void MemoryTexture::init(const ImageData &src)
	{
		init(src.w, src.h, src.format);
		updateRegion(0, 0, src);
	}

	void MemoryTexture::init(int w, int h, TextureFormat Format)
	{
		int bytesPerPixel = getBytesPerPixel(Format);

		_buffer.resize(h * w * bytesPerPixel);
		_image = ImageData(w, h, w * bytesPerPixel, Format, &_buffer.front());
	}

	

	int	MemoryTexture::getWidth() const 
	{
		return _image.w;
	}

	int	MemoryTexture::getHeight() const 
	{
		return _image.h;
	}

	TextureFormat MemoryTexture::getFormat() const 
	{
		return _image.format;
	}

	ImageData MemoryTexture::lock(lock_flags, const Rect *pRect)
	{
		Rect rect(0, 0, _image.w, _image.h);
		if (pRect)	
		{
			rect = *pRect;
			OX_ASSERT(rect.getX() + rect.getWidth() <= _image.w);
			OX_ASSERT(rect.getY() + rect.getHeight() <= _image.h);
		}

		ImageData im = _image;

		void *ptr = &_buffer.front() + rect.getX() * _image.bytespp + rect.getY() * _image.pitch + _offset;

		return ImageData (rect.getWidth(), rect.getHeight(), _image.pitch, _image.format, ptr);
	}

	ImageData MemoryTexture::lock(const Rect *pRect)
	{
		return lock(lock_read | lock_write, pRect);
	}

	void MemoryTexture::unlock()
	{

	}

	void MemoryTexture::updateRegion(int x, int y, const ImageData &src)
	{
		Rect r(x, y, src.w, src.h);
		ImageData dest = lock(&r);
		operations::blit(src, dest);
		unlock();
	}

	void MemoryTexture::apply(const Rect *)
	{

	}


	void MemoryTexture::swap(MemoryTexture &r)
	{
		ImageData copy = _image;
		_image = r._image;
		r._image = copy;

		std::swap(_buffer, r._buffer);
	}
}