#include "ImageData.h"
#include "utils/stringUtils.h"
namespace oxygine
{
    int getBytesPerPixel(TextureFormat tf)
    {
        switch (tf)
        {
            case TF_A8:
            case TF_L8:
                return 1;
            case TF_A8L8:
                return 2;
            case TF_R8G8B8A8:
            case TF_B8G8R8A8:
                return 4;
            case TF_R8G8B8:
                return 3;
            case TF_R5G5B5A1:
                return 2;
            case TF_R5G6B5:
                return 2;
            case TF_R4G4B4A4:
                return 2;
            case TF_PVRTC_2RGB:
            case TF_PVRTC_2RGBA:
            case TF_PVRTC_4RGB:
            case TF_PVRTC_4RGBA:
            case TF_PVRTCII_2:
            case TF_PVRTCII_4:
            case TF_ETC1:
                return 0;
            case TF_UNDEFINED:
                return 0;
            default:
                OX_ASSERT(!"unknown type");
        }
        return 0;
    }

    bool isCompressedFormat(TextureFormat tf)
    {
        switch (tf)
        {
            case TF_PVRTC_2RGB:
            case TF_PVRTC_2RGBA:
            case TF_PVRTC_4RGB:
            case TF_PVRTC_4RGBA:
            case TF_PVRTCII_2:
            case TF_PVRTCII_4:
            case TF_ETC1:
                return true;
        }
        return false;
    }

    TextureFormat string2TextureFormat(const char* str)
    {
#define CMP(fmt, shrt) if (!strcmp_cns(str, #fmt) || (shrt && !strcmp_cns(str, shrt))) return TF_##fmt
        CMP(A8, 0);
        CMP(L8, 0);
        CMP(A8L8, 0);
        CMP(R8G8B8, "888");
        CMP(R8G8B8A8, "8888");
        CMP(R5G5B5A1, "5551");
        CMP(R4G4B4A4, "4444");
        CMP(R5G6B5, "565");
        CMP(PVRTC_2RGB, 0);
        CMP(PVRTC_2RGBA, 0);
        CMP(PVRTC_4RGB, 0);
        CMP(PVRTC_4RGBA, 0);
        CMP(PVRTCII_2, 0);
        CMP(PVRTCII_4, 0);
        CMP(ETC1, 0);
#undef  CMP
        OX_ASSERT(!"string2TextureFormat undefined format");
        return TF_UNDEFINED;
    }

    const char* textureFormat2String(TextureFormat tf)
    {
        switch (tf)
        {
            case TF_A8:
                return "A8";
            case TF_L8:
                return "L8";
            case TF_A8L8:
                return "A8L8";
            case TF_R8G8B8A8:
                return "R8G8B8A8";
            case TF_R8G8B8:
                return "R8G8B8";
            case TF_R5G5B5A1:
                return "R5G5B5A1";
            case TF_R5G6B5:
                return "R5G6B5";
            case TF_R4G4B4A4:
                return "R4G4B4A4";
            case TF_PVRTC_2RGB:
                return "PVRTC_2RGB";
            case TF_PVRTC_2RGBA:
                return "PVRTC_2RGBA";
            case TF_PVRTC_4RGB:
                return "PVRTC_4RGB";
            case TF_PVRTC_4RGBA:
                return "PVRTC_4RGBA";
            case TF_PVRTCII_2:
                return "PVRTCII_2";
            case TF_PVRTCII_4:
                return "PVRTCII_4";
            case TF_ETC1:
                return "ETC1";
            case TF_UNDEFINED:
                return "undefined";
            default:
                OX_ASSERT(!"unknown");
        }
        return "unknown";
    }

    ImageData::ImageData(): w(0), h(0), bytespp(0), pitch(0), data(0), format(TF_UNDEFINED)
    {

    }

    ImageData::ImageData(int W, int H, int Pitch, TextureFormat Format, void* Data): w(W), h(H), pitch(Pitch), format(Format), data((unsigned char*)Data)
    {
        bytespp = getBytesPerPixel(Format);
    }

    ImageData::ImageData(const ImageData& b, void* Data)
    {
        *this = ImageData(b.w, b.h, b.pitch, b.format, Data);
    }

    ImageData::~ImageData()
    {
    }

    ImageData ImageData::getRect(const Rect& r) const
    {
        OX_ASSERT(r.getX() + r.getWidth() <= w);
        OX_ASSERT(r.getY() + r.getHeight() <= h);

        void* ptr = (unsigned char*)data + r.getX() * bytespp + r.getY() * pitch;
        ImageData buffer(r.getWidth(), r.getHeight(), pitch, format, ptr);

        return buffer;
    }

    unsigned char* ImageData::getPixelPtr(int x, int y) const
    {
        return (unsigned char*)data + x * bytespp + y * pitch;
    }
}
