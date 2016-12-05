#pragma once
#include "oxygine-include.h"
#include "core/Texture.h"
#include "core/file.h"

namespace oxygine
{
    enum ImageType
    {
        IT_UNKNOWN,
        IT_PNG,
        IT_PKM,
        IT_PVR2,
        IT_PVR,
        IT_TGA,
        IT_JPEG
    };

    bool getImageInfo(const void* data, size_t size, const char* name, ImageType& type, int& width, int& height);

    DECLARE_SMART(Image, spImage);

    class Image : public Texture
    {
    public:
        Image();
        ~Image();

        bool init(file::buffer& bf, bool premultiplied = false, TextureFormat format = TF_UNDEFINED);
        void init(const ImageData& src);
        void init(int w, int h, TextureFormat Format);

        void cleanup();
        void convert(Image& dest, TextureFormat format);
        //void convert2pot(MemoryTexture &dest);

        OXYGINE_DEPRECATED
        void fill_zero();

        void fillZero() { fill(0); }
        void fill(unsigned int val);

        unsigned int    getSizeVRAM() const {return (unsigned int)_buffer.size();}
        int             getWidth() const;
        int             getHeight() const;
        const Point&    getSize() const;
        TextureFormat   getFormat() const;

        ImageData   lock(lock_flags f = 0, const Rect* pRect = 0);
        ImageData   lock(const Rect* pRect);
        ImageData   lock(const Rect& pRect);
        ImageData   lock(int x, int y, int w, int h);
        ImageData   lock(int x, int y);

        void        unlock();
        void        toPOT(Image& dest);

        void        updateRegion(int x, int y, const ImageData& data);
        void        apply(const Rect*);

        void        swap(Image& r);

    private:
        ImageData _image;
        size_t _offset;//buffer offset
        std::vector<unsigned char> _buffer;
    };

    typedef bool (*cbLoadImageFromBuffer)(Image& mt, void* data, int nSize, bool premultiplied, TextureFormat format);
    void setJpegImageLoader(cbLoadImageFromBuffer);
    void setPngImageLoader(cbLoadImageFromBuffer);
    void setCustomImageLoader(cbLoadImageFromBuffer);
}