#pragma once
#include "oxygine-include.h"
#include "../NativeTexture.h"

namespace oxygine
{
    DECLARE_SMART(NativeTextureGLES, spNativeTextureGLES);
    class NativeTextureGLES : public NativeTexture
    {
    public:
        ~NativeTextureGLES();

        void init(nativeTextureHandle id, int w, int h, TextureFormat tf);
        void init(int w, int h, TextureFormat tf, bool renderTarget);
        void init(const ImageData& src, bool sysMemCopy);
        void release();
        void swap(NativeTexture*) ;

        nativeTextureHandle getHandle() const;
        int                 getWidth() const;
        int                 getHeight() const;
        TextureFormat       getFormat() const;
        unsigned int        getFboID() const;

        ImageData lock(lock_flags, const Rect* src);
        void unlock();

        void setLinearFilter(bool enable);
        void setClamp2Edge(bool clamp2edge);

        void updateRegion(int x, int y, const ImageData& data);
        void apply(const Rect* rect = 0);

    protected:
        void* _getRestorableObject() {return this;}
        friend class VideoDriverGL;
        friend class VideoDriverGLES20;
        NativeTextureGLES();

        size_t _id;
        size_t _fbo;

        TextureFormat _format;
        int _width;
        int _height;
        ImageData _im;

        std::vector<unsigned char> _data;
        int _lockFlags;
        Rect _lockRect;
    };
}