#pragma once
#include "oxygine_include.h"
#include "Resource.h"
#include "AnimationFrame.h"
#include "core/Texture.h"
#include <vector>

namespace oxygine
{
    DECLARE_SMART(Texture, spTexture);


    typedef std::vector<AnimationFrame> animationFrames;
    class MemoryTexture;

    DECLARE_SMART(ResAnim, spResAnim);
    class ResAnim: public _Resource
    {
    public:
        ResAnim(Resource* atlas = 0);
        ~ResAnim();

        void init(const std::string& file, int columns = 1, int rows = 1, float scaleFactor = 1.0f);
        void init(MemoryTexture* original, int columns = 1, int rows = 1, float scaleFactor = 1.0f);
        void init(animationFrames& frames, int columns, float scaleFactor = 1.0f, float appliedScale = 1.0f);
        /**creates animation frames from NativeTexture*/
        void init(spNativeTexture texture, const Point& originalSize, int columns, int rows, float scaleFactor);

        /*adds additional column. use it only if rows = 1*/
        //void addFrame(const AnimationFrame &frame);

        float                   getScaleFactor() const {return _scaleFactor;}
        float                   getAppliedScale() const { return _appliedScale; }
        int                     getColumns() const {return _columns;}
        int                     getRows() const {return (int)_frames.size() / _columns;}
        int                     getTotalFrames() const {return (int)_frames.size();}
        const AnimationFrame&   getFrame(int col, int row) const;
        /**returns frame by index ignoring cols and rows*/
        const AnimationFrame&   getFrame(int index) const;
        /**Returns atlas where this ResAnim was created*/
        Resource*               getAtlas() {return _atlas;}
        /**Returns size of frame*/
        const Vector2&          getSize() const;
        float                   getWidth() const;
        float                   getHeight() const;

        void setFrame(int col, int row, const AnimationFrame& frame);

        operator const AnimationFrame& ();

    protected:

        void _load(LoadResourcesContext* ctx = 0);
        void _unload();

        int         _columns;
        Resource*   _atlas;
        float       _scaleFactor;
        float       _appliedScale;

        animationFrames _frames;
    };
}
