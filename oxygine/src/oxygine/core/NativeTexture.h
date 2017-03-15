#pragma once
#include "../oxygine-include.h"
#include "../closure/closure.h"
#include "Restorable.h"
#include "ox_debug.h"
#include "Texture.h"

namespace oxygine
{
    class NativeTexture: public Texture, public Restorable
    {
    public:
        NativeTexture() {}
        virtual void init(nativeTextureHandle, int w, int h, TextureFormat tf) = 0;
        virtual void init(int w, int h, TextureFormat tf, bool renderTarget = false) = 0;
        virtual void init(const ImageData& src, bool sysMemCopy = false) = 0;
        //virtual void release() = 0;

        virtual ImageData lock(lock_flags, const Rect* src) = 0;
        virtual void unlock() = 0;
        virtual void swap(NativeTexture*) = 0;

        /**Disabled/Enables bilinear texture filter*/
        virtual void setLinearFilter(bool enable) = 0;
        /**set wrap mode as clamp2edge or repeat (tiling)*/
        virtual void setClamp2Edge(bool clamp2edge) = 0;
        virtual void updateRegion(int x, int y, const ImageData& data) = 0;
        virtual void apply(const Rect* rect = 0) = 0;

        /**returns handle (ptr) to HW texture ID*/
        virtual nativeTextureHandle getHandle() const = 0;

        //debug
        static void dumpCreatedTextures();
        static std::vector<spNativeTexture> getCreatedTextures();
        /**debug counter of created textures*/
        static volatile int created;
    };

    class NativeTextureNull: public NativeTexture
    {
    public:
        void init(nativeTextureHandle, int w, int h, TextureFormat tf);
        void init(int w, int h, TextureFormat tf, bool renderTarget = false);
        void init(const ImageData& src, bool sysMemCopy);
        void release();

        ImageData lock(lock_flags, const Rect* src);
        void unlock();
        void swap(NativeTexture*);

        void setClamp2Edge(bool clamp2edge);
        void setLinearFilter(bool enable);
        void updateRegion(int x, int y, const ImageData& data);
        void apply(const Rect* rect = 0);

        /**returns handle (ptr) to HW texture ID*/
        nativeTextureHandle getHandle() const;
        int getWidth() const;
        int getHeight() const;
        TextureFormat getFormat() const;

        void* _getRestorableObject() { return 0; }
    };
}