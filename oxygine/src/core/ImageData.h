#pragma once
#include "oxygine_include.h"
#include "math/Rect.h"
#include "pixel.h"

namespace oxygine
{
    enum TextureFormat
    {
        TF_UNDEFINED,
        TF_A8,//luminance
        TF_L8,//luminance
        TF_A8L8,//luminance+alpha
        TF_R8G8B8A8,//default
        TF_B8G8R8A8,
        TF_R8G8B8,
        TF_R5G5B5A1,
        TF_R4G4B4A4,
        TF_R5G6B5,
        TF_B5G6R5,
        TF_PVRTC_2RGB,
        TF_PVRTC_2RGBA,
        TF_PVRTC_4RGB,
        TF_PVRTC_4RGBA,
        TF_PVRTCII_2,
        TF_PVRTCII_4,
        TF_ETC1,
    };

#define ALL_FORMATS_SWITCH(format) \
    switch(format) \
    { \
            FORMAT_CASE(A8); \
            FORMAT_CASE(L8); \
            FORMAT_CASE(A8L8); \
            FORMAT_CASE(R8G8B8A8); \
            FORMAT_CASE(B8G8R8A8); \
            FORMAT_CASE(R8G8B8); \
            FORMAT_CASE(R5G5B5A1); \
            FORMAT_CASE(R4G4B4A4); \
            FORMAT_CASE(R5G6B5); \
            FORMAT_CASE(B5G6R5); \
        default: \
            OX_ASSERT(!"unknown format"); \
    }

    int getBytesPerPixel(TextureFormat tf);
    const char* textureFormat2String(TextureFormat f);
    TextureFormat string2TextureFormat(const char* str);
    bool isCompressedFormat(TextureFormat tf);



    class ImageData
    {
    public:
        ImageData();
        ImageData(int W, int H, int Pitch, TextureFormat Format, void* Data = 0);
        ImageData(const ImageData& b, void* Data);
        ~ImageData();

        ImageData getRect(const Rect& r) const;
        ImageData getRect(int x, int y, int w, int h) const;
        ImageData getRect(int x, int y) const;
        unsigned char* getPixelPtr(int x, int y) const;

    public:
        int w;
        int h;
        int bytespp;
        int pitch;

        unsigned char* data;
        TextureFormat format;
    };
}
