#include "Image.h"
#include "math/Rect.h"
#include "core/ImageDataOperations.h"
#include "core/file.h"
#include "core/log.h"
#include "utils/ImageUtils.h"
#include <stdint.h>

#ifdef __APPLE__
#include "core/ios/ios.h"
#endif


extern "C"
{
#ifdef OX_HAVE_LIBPNG
#include "png.h"
#endif

#ifdef OX_HAVE_LIBJPEG
#include "jpeglib.h"
#endif // OX_HAVE_LIBJPEG
};

//#define  LOGD(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)

namespace oxygine
{


    bool loadImageNotSupported(Image& mt, void* data, int nSize, bool premultiplied, TextureFormat format)
    {
        return false;
    }

    bool isPngSource(const unsigned char* data, size_t size)
    {
        return data[0] == 0x89 && data[1] == 'P' && data[2] == 'N' && data[3] == 'G';
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


    struct pvr_header2
    {
        uint32_t headerLength;
        uint32_t height;
        uint32_t width;
        uint32_t numMipmaps;
        uint32_t flags;
        uint32_t dataLength;
        uint32_t bpp;
        uint32_t bitmaskRed;
        uint32_t bitmaskGreen;
        uint32_t bitmaskBlue;
        uint32_t bitmaskAlpha;
        uint32_t pvrTag;
        uint32_t numSurfs;
    } PVRTexHeader;

    struct pvr_header
    {
        uint32_t version;
        uint32_t flags;
        uint32_t formatA;
        uint32_t formatB;
        uint32_t color_space;
        uint32_t channel_type;
        uint32_t height;
        uint32_t width;
        uint32_t depth;
        uint32_t num_surfaces;
        uint32_t num_faces;
        uint32_t num_mips;
        uint32_t meta_data_size;
    };

#pragma pack(push, 1)
    struct tga_header
    {
        char  idlength;
        char  colourmaptype;
        char  datatypecode;
        short int colourmaporigin;
        short int colourmaplength;
        char  colourmapdepth;
        short int x_origin;
        short int y_origin;
        short width;
        short height;
        char  bitsperpixel;
        char  imagedescriptor;
    };
#pragma pack(pop)

    unsigned short swapByteOrder(unsigned short us)
    {
        return (us >> 8) | (us << 8);
    }

    ImageType getImageType(const void* Data, size_t size)
    {
        const unsigned char* data = (const unsigned char*)Data;
        const unsigned int& dt = *((unsigned int*)data);
        if (isPngSource(data, size))
            return IT_PNG;
        if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF)
            return IT_JPEG;
        if (data[0] == 'P' && data[1] == 'K' && data[2] == 'M')
            return IT_PKM;
        if (dt == 0x03525650)
            return IT_PVR;
        if (data[0] == 0 && data[1] == 0 && data[2] == 2)
            return IT_TGA;

        if ((&dt)[11] == 0x21525650)
            return IT_PVR2;

        return IT_UNKNOWN;
    }

    int int32FromNetworkByteOrder(const unsigned char* bf)
    {
        return bf[3] | (bf[2] << 8) | (bf[1] << 16) | (bf[0] << 24);
    }

    bool getImageInfo(const void* Data, size_t size, const char* name, ImageType& type, int& width, int& height)
    {
        const unsigned char* data = reinterpret_cast<const unsigned char*>(Data);

        type = IT_UNKNOWN;
        width = 0;
        height = 0;

        type = getImageType(data, size);

        switch (type)
        {
            case IT_PNG:
                width = int32FromNetworkByteOrder(data + 16);
                height = int32FromNetworkByteOrder(data + 20);
                break;

            case IT_PVR:
            {
                const pvr_header* header = (const pvr_header*)data;
                width = header->width;
                height = header->height;
            }
            break;
            case IT_PVR2:
            {
                const pvr_header2* header = (const pvr_header2*)data;
                width = header->width;
                height = header->height;
            }
            break;
            case IT_PKM:
            {
                const pkm_header* header = (const pkm_header*)data;
                width = swapByteOrder(header->width);
                height = swapByteOrder(header->height);
            }
            break;
            case IT_TGA:
            {
                const tga_header* header = (const tga_header*)data;
                width = header->width;
                height = header->height;
            }
            break;
            default:
                log::error("unknown file format to read size");
                return false;
        }

        return true;
    }

#ifdef OX_HAVE_LIBJPEG
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
            return FALSE;
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

    bool _initWithJpgData(Image& mt, void* data, int nSize, bool premultiplied, TextureFormat format)
    {
        bool bRet = false;


        jpeg_decompress_struct cinfo;
        memset(&cinfo, 0, sizeof(cinfo));

        JSAMPARRAY buffer;      /* Output row buffer */
        int row_stride;     /* physical row width in output buffer */

        jpeg_source_mgr srcmgr;

        srcmgr.bytes_in_buffer = nSize;
        srcmgr.next_input_byte = (JOCTET*)data;
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
                 ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

        int copy_rows = (int)cinfo.output_height;
        int copy_width = (int)cinfo.output_width;

        if (copy_width < 0 || copy_rows < 0)
        {
            //printf("jpeg is fully off screen\n");
            return bRet;
        }
        int starty = 0;

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
            (void)jpeg_read_scanlines(&cinfo, buffer, 1);

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

        (void)jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        //printf("jpeg display done\n");

        bRet = true;
        return bRet;
    }
#endif // OX_HAVE_LIBJPEG





#ifdef OX_HAVE_LIBPNG
    struct imageSource
    {
        unsigned char* data;
        int size;
        int offset;
    };

    static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        imageSource* isource = (imageSource*)png_get_io_ptr(png_ptr);

        LOGD("png read l: %d, %d", length, isource->offset);

        if ((int)(isource->offset + length) <= isource->size)
        {
            memcpy(data, isource->data + isource->offset, length);
            isource->offset += length;
        }
        else
        {
            log::error("png error");
            png_error(png_ptr, "pngReaderCallback failed");
        }
    }



    bool _initWithPngData(Image& mt, void* pData, int nDatalen, bool premultiplied, TextureFormat format)
    {
        LOGD("reading png...");
        bool bRet = false;
        png_structp     png_ptr     =   0;
        png_infop       info_ptr    = 0;

        do
        {

            // init png_struct
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
            LOGD("reading png...1");

            // init png_info
            info_ptr = png_create_info_struct(png_ptr);


//#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
            setjmp(png_jmpbuf(png_ptr));
//#endif
            // set the read call back function
            imageSource imageSource;
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
                destFormat = TF_R8G8B8A8;

            LOGD("reading png...4, %d %d", nWidth, nHeight);
            mt.init(nWidth, nHeight, destFormat);

            ImageData dest = mt.lock();

            png_bytep* rowPointers = png_get_rows(png_ptr, info_ptr);

            LOGD("reading png...5");
            int pitch = (int)png_get_rowbytes(png_ptr, info_ptr);
            ImageData src(nWidth, 1, pitch, srcFormat,  0);

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
        }
        while (0);


        if (png_ptr)
        {
            png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);

        }
        LOGD("reading png...7");
        return bRet;
    }

#endif

#if __APPLE__
#define USE_NSIMAGE  0
#endif

#if USE_NSIMAGE
    cbLoadImageFromBuffer _loadPngImage = nsImageLoad;
#elif OX_HAVE_LIBPNG
    cbLoadImageFromBuffer _loadPngImage = _initWithPngData;
#else
    cbLoadImageFromBuffer _loadPngImage = loadImageNotSupported;
#endif

#if USE_NSIMAGE
    cbLoadImageFromBuffer _loadJpegImage = nsImageLoad;
#elif OX_HAVE_LIBJPEG
    cbLoadImageFromBuffer _loadJpegImage = _initWithJpgData;
#else
    cbLoadImageFromBuffer _loadJpegImage = loadImageNotSupported;
#endif


    cbLoadImageFromBuffer _loadCustomImage = loadImageNotSupported;



    void setJpegImageLoader(cbLoadImageFromBuffer cb)
    {
        _loadJpegImage = cb;
    }

    void setPngImageLoader(cbLoadImageFromBuffer cb)
    {
        _loadPngImage = cb;
    }

    void setCustomImageLoader(cbLoadImageFromBuffer cb)
    {
        _loadCustomImage = cb;
    }

    bool loadPngImage(Image& mt, void* pData, int nDatalen, bool premultiplied, TextureFormat format)
    {
        bool s = _loadPngImage(mt, pData, nDatalen, premultiplied, format);
        return s;
    }

    bool loadJpegImage(Image& mt, void* pData, int nDatalen, bool premultiplied, TextureFormat format)
    {
        bool s = _loadJpegImage(mt, pData, nDatalen, premultiplied, format);
        return s;
    }


    Image::Image(): _offset(0)
    {

    }

    Image::~Image()
    {

    }

    void Image::cleanup()
    {
        _buffer.clear();
        _image = ImageData();
    }

    void Image::convert(Image& dest, TextureFormat format)
    {
        dest.init(getWidth(), getHeight(), format);
        ImageData src = lock();
        ImageData dst = dest.lock();

        operations::blit(src, dst);
    }

    void Image::fill_zero()
    {
        if (_buffer.empty())
            return;
        memset(&_buffer.front(), 0, _buffer.size());
    }

    void Image::fill(unsigned int val)
    {
        if (_buffer.empty())
            return;
        memset(&_buffer.front(), val, _buffer.size());
    }

    bool Image::init(file::buffer& buffer, bool premultiplied, TextureFormat format)
    {
        cleanup();

        if (buffer.getSize())
        {
            ImageType type = getImageType(buffer.getData(), buffer.getSize());
            switch (type)
            {
                case IT_PNG:
                    if (loadPngImage(*this, (void*)buffer.getData(), buffer.getSize(), premultiplied, format))
                        return true;
                    break;
                case IT_JPEG:
                    if (loadJpegImage(*this, (void*)buffer.getData(), buffer.getSize(), premultiplied, format))
                        return true;
                    break;
                case IT_PKM:
                {
                    const pkm_header* header = (const pkm_header*)buffer.getData();
                    _image.w = swapByteOrder(header->width);
                    _image.h = swapByteOrder(header->height);
                    int or_w = swapByteOrder(header->original_width);
                    int or_h = swapByteOrder(header->original_height);
                    OX_ASSERT(_image.w == or_w && _image.h == or_h);

                    _image.bytespp = 0;
                    _image.format = TF_ETC1;
                    _offset = sizeof(pkm_header);
                    _image.pitch = int(buffer.getSize() - _offset) / _image.h;
                    _buffer.swap(buffer.data);
                }
                return true;
                case IT_PVR:
                {
                    const pvr_header* header = (const pvr_header*)buffer.getData();
                    _image.w = header->width;
                    _image.h = header->height;
                    _image.bytespp = 0;

                    TextureFormat tf = TF_UNDEFINED;
                    switch (header->formatA)
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
                    _offset = sizeof(*header) + header->meta_data_size;
                    _image.pitch = int(buffer.getSize() - _offset) / _image.h;
                    _buffer.swap(buffer.data);
                    return true;
                }
                break;
                case IT_PVR2:
                {
                    const pvr_header2* header = (const pvr_header2*)buffer.getData();
                    _image.w = header->width;
                    _image.h = header->height;
                    _image.bytespp = 0;


                    TextureFormat tf = TF_UNDEFINED;

                    int ff = header->flags & 0xff;
                    switch (ff)
                    {
                        case 16:
                            tf = TF_R4G4B4A4;
                            break;
                        case 17:
                            tf = TF_R5G5B5A1;
                            break;
                        case 18:
                            tf = TF_R8G8B8A8;
                            break;
                        case 19:
                            tf = TF_R5G6B5;
                            break;
                        case 21:
                            tf = TF_R8G8B8;
                            break;
                        case 24:
                            tf = header->bitmaskAlpha ? TF_PVRTC_2RGBA : TF_PVRTC_2RGB;
                            break;
                        case 25:
                            tf = header->bitmaskAlpha ? TF_PVRTC_4RGBA : TF_PVRTC_4RGB;
                            break;
                        default:
                            log::error("unknown pvr2 flag format");
                    }

                    _image.format = tf;
                    _offset = sizeof(*header);
                    _image.pitch = int(buffer.getSize() - _offset) / _image.h;
                    _buffer.swap(buffer.data);
                    return true;

                }
                break;
                case IT_TGA:
                {
                    const tga_header* header = (const tga_header*)buffer.getData();
                    if (header->bitsperpixel == 32)
                    {
                        int pitch = header->width * 4;

                        ImageData src;
                        if (!(header->imagedescriptor & 32))
                        {
                            src = ImageData(header->width, header->height, -pitch, TF_B8G8R8A8, (char*)buffer.getData() + sizeof(tga_header) + (header->height - 1) * pitch);
                        }
                        else
                        {
                            src = ImageData(header->width, header->height, pitch, TF_B8G8R8A8, (char*)buffer.getData() + sizeof(tga_header));
                        }

                        init(header->width, header->height, format);

                        if (premultiplied)
                        {
                            operations::op_premultipliedAlpha op;
                            ImageData dest = lock();
                            applyOperation(op, src, dest);
                        }
                        else
                        {
                            updateRegion(0, 0, src);
                        }

                        return true;
//                      saveImage(lock(), "test.png", "png");
                    }
                    else
                    {
                        log::warning("tga 32bpp only supported");
                    }
                    break;
                }
                break;

                case IT_UNKNOWN:
                {
                    if (_loadCustomImage(*this, (void*)buffer.getData(), buffer.getSize(), premultiplied, format))
                        return true;
                }
                break;
                default:
                    break;
            }
        }

        log::warning("Image. can't unpack data unknown file format");

        init(16, 16, TF_R8G8B8A8);
        fill_zero();

        return false;
    }

    void Image::init(const ImageData& src)
    {
        init(src.w, src.h, src.format);
        updateRegion(0, 0, src);
    }

    void Image::init(int w, int h, TextureFormat Format)
    {
        int bytesPerPixel = getBytesPerPixel(Format);

        _buffer.resize(h * w * bytesPerPixel);
        _image = ImageData(w, h, w * bytesPerPixel, Format, &_buffer.front());
    }



    int Image::getWidth() const
    {
        return _image.w;
    }

    int Image::getHeight() const
    {
        return _image.h;
    }

    const Point& Image::getSize() const
    {
        return *((Point*)&_image.w);
    }

    TextureFormat Image::getFormat() const
    {
        return _image.format;
    }

    ImageData Image::lock(lock_flags, const Rect* pRect)
    {
        Rect rect(0, 0, _image.w, _image.h);
        if (pRect)
        {
            rect = *pRect;
            OX_ASSERT(rect.getX() >= 0);
            OX_ASSERT(rect.getY() >= 0);
            OX_ASSERT(rect.getX() + rect.getWidth() <= _image.w);
            OX_ASSERT(rect.getY() + rect.getHeight() <= _image.h);
        }

        ImageData im = _image;

        void* ptr = &_buffer.front() + rect.getX() * _image.bytespp + rect.getY() * _image.pitch + _offset;

        return ImageData(rect.getWidth(), rect.getHeight(), _image.pitch, _image.format, ptr);
    }

    ImageData Image::lock(const Rect* pRect)
    {
        return lock(lock_read | lock_write, pRect);
    }

    ImageData Image::lock(const Rect& rect)
    {
        return lock(lock_read | lock_write, &rect);
    }

    void Image::unlock()
    {

    }

    void Image::updateRegion(int x, int y, const ImageData& src)
    {
        Rect r(x, y, src.w, src.h);
        ImageData dest = lock(&r);
        operations::blit(src, dest);
        unlock();
    }

    void Image::apply(const Rect*)
    {

    }


    void Image::swap(Image& r)
    {
        ImageData copy = _image;
        _image = r._image;
        r._image = copy;

        std::swap(_buffer, r._buffer);
    }
}
