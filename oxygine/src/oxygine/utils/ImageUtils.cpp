#include "ImageUtils.h"
#include "core/ImageData.h"
#include "Image.h"
//#include "png.h"
extern "C"
{
#ifdef OX_HAVE_LIBJPEG
#include "jpeglib.h"
#endif
};


#ifdef __S3E__
#include "IwImage.h"
#endif

namespace oxygine
{
    void saveImage(const ImageData& im_, const char* path, const char* format)
    {
        Image src;
        src.init(im_);
        Image dest;
        src.convert(dest, TF_B8G8R8A8);
        const ImageData& im = dest.lock();

        file::handle h = file::open(path, "wb");
        file::autoClose ac(h);
        char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        header[12] = im.w & 0xFF;
        header[13] = (im.w >> 8) & 0xFF;
        header[14] = (im.h) & 0xFF;
        header[15] = (im.h >> 8) & 0xFF;
        header[16] = im.bytespp * 8;
        file::write(h, header, 18);

        const unsigned char* end = im.data + im.w * im.h * im.bytespp;
        int line = im.w * im.bytespp;

        for (int y = 0; y < im.h; ++y)
        {
            end -= line;
            file::write(h, end, line);
        }
    }

    void serializeImage(const ImageData& im, file::buffer& bf, const char* format)
    {
#ifdef OX_HAVE_LIBJPEG
        char* filename = (char*)"test_jpeg.jpg";
        int quality    = 50;

        struct jpeg_compress_struct cinfo;    // basic info for JPEG properties
        struct jpeg_error_mgr jerr;           // in case of error
        FILE* outfile = 0;                        // target file
        JSAMPROW row_pointer[1];              // pointer to JSAMPLE row[s]
        int row_stride;                       // physical row width in image buffer

        //## ALLOCATE AND INITIALIZE JPEG COMPRESSION OBJECT

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        //## OPEN FILE FOR DATA DESTINATION:
        /*
        if ((outfile = fopen(filename, "wb")) == NULL) {
            fprintf(stderr, "ERROR: can't open %s\n", filename);
            exit(1);
        }
        */

        cinfo.dest = 0;

        //## SET PARAMETERS FOR COMPRESSION:

        cinfo.image_width  = 20;           // |-- image width and height in pixels
        cinfo.image_height = 20;           // |
        cinfo.input_components = 3;         // number of color components per pixel
        cinfo.in_color_space = JCS_RGB;     // colorspace of input image as RGB

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE);


        //## CREATE IMAGE BUFFER TO WRITE FROM AND MODIFY THE IMAGE TO LOOK LIKE CHECKERBOARD:

        unsigned char* image_buffer = NULL;
        image_buffer = (unsigned char*)malloc(cinfo.image_width * cinfo.image_height * cinfo.num_components);

        for (size_t y = 0; y < cinfo.image_height; y++)
            for (size_t x = 0; x < cinfo.image_width; x++)
            {
                unsigned int pixelIdx = (unsigned int)(((y * cinfo.image_height) + x) * cinfo.input_components);

                if (x % 2 == y % 2)
                {
                    image_buffer[pixelIdx + 0] = 255; // r |-- set r,g,b components to
                    image_buffer[pixelIdx + 1] = 0;   // g |   make this pixel red
                    image_buffer[pixelIdx + 2] = 0;   // b |   (255,0,0)
                }
                else
                {
                    image_buffer[pixelIdx + 0] = 255; // r |-- set r,g,b components to
                    image_buffer[pixelIdx + 1] = 255; // g |   make this pixel white
                    image_buffer[pixelIdx + 2] = 255; // b |   (255,255,255)
                }
            }

        //## START COMPRESSION:

        jpeg_start_compress(&cinfo, TRUE);
        row_stride = cinfo.image_width * 3;        // JSAMPLEs per row in image_buffer

        while (cinfo.next_scanline < cinfo.image_height)
        {
            row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
        // NOTE: jpeg_write_scanlines expects an array of pointers to scanlines.
        //       Here the array is only one element long, but you could pass
        //       more than one scanline at a time if that's more convenient.

        //## FINISH COMPRESSION AND CLOSE FILE:

        jpeg_finish_compress(&cinfo);
        fclose(outfile);
        jpeg_destroy_compress(&cinfo);

        printf("SUCCESS\n");
#endif

    }
}
