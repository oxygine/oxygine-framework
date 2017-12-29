#pragma once
#include "../oxygine-include.h"
#include "ref_counter.h"
#include "vertex.h"
#include "../math/Matrix.h"
#include "../math/Rect.h"
#include "../math/Vector4.h"
#include <vector>

namespace oxygine
{
    class IVideoDriver
    {
    public:

        enum PRIMITIVE_TYPE
        {
            PT_POINTS,
            PT_LINES,
            PT_LINE_LOOP,
            PT_LINE_STRIP,
            PT_TRIANGLES,
            PT_TRIANGLE_STRIP,
            PT_TRIANGLE_FAN,
            PT_COUNT,
        };


        class Stats
        {
        public:
            Stats() : batches(0), duration(0), start(0) { memset(elements, 0, sizeof(elements)); }
            int batches;
            int elements[PT_COUNT];
            timeMS start;
            timeMS duration;
        };
        static Stats _stats;

        enum BLEND_TYPE
        {
            BT_ZERO,
            BT_ONE,
            BT_SRC_COLOR,
            BT_ONE_MINUS_SRC_COLOR,
            BT_SRC_ALPHA,
            BT_ONE_MINUS_SRC_ALPHA,
            BT_DST_ALPHA,
            BT_DST_COLOR,
            BT_ONE_MINUS_DST_ALPHA,
            BT_ONE_MINUS_DST_COLOR,
        };

        enum CULL_FACE_TYPE
        {
            CULL_FACE_FRONT_AND_BACK,
            CULL_FACE_FRONT,
            CULL_FACE_BACK,
        };

        enum STATE
        {
            STATE_BLEND,
            STATE_CULL_FACE,
            STATE_NUM
        };

        static IVideoDriver* instance;
        virtual ~IVideoDriver();

        virtual void reset() = 0;
        virtual void restore() = 0;
        virtual bool isReady() const = 0;

        virtual spNativeTexture createTexture() = 0;

        virtual void clear(const Color& color) = 0;
        virtual void begin(const Rect& viewport, const Color* color) = 0;
        virtual void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize) = 0;
        virtual void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize, const unsigned short* indicesData, unsigned int numIndices) = 0;


        virtual void            getViewport(Rect& r) const = 0;
        virtual bool            getScissorRect(Rect&) const = 0;
        virtual spNativeTexture getRenderTarget() const = 0;
        virtual ShaderProgram*  getShaderProgram() const = 0;
        virtual const VertexDeclaration* getVertexDeclaration(bvertex_format) const = 0;

        virtual void setScissorRect(const Rect*) = 0;
        virtual void setDefaultSettings() = 0;
        virtual void setViewport(const Rect& viewport) = 0;
        virtual void setRenderTarget(spNativeTexture) = 0;
        virtual void setShaderProgram(ShaderProgram*) = 0;
        virtual void setTexture(int sampler, spNativeTexture) = 0;
        virtual void setState(STATE, unsigned int value) = 0;
        virtual void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest) = 0;

        virtual void setUniform(const char* id, const Vector4* v, int num) = 0;
        virtual void setUniform(const char* id, const Vector3* v, int num) = 0;
        virtual void setUniform(const char* id, const Vector2* v, int num) = 0;
        virtual void setUniform(const char* id, const Matrix* v, int num) = 0;
        virtual void setUniform(const char* id, float v) = 0;
        virtual void setUniformInt(const char* id, int v) = 0;

        void setUniform(const char* id, const Vector4& v);
        void setUniform(const char* id, const Vector3& v);
        void setUniform(const char* id, const Vector2& v);
        void setUniform(const char* id, const Matrix&  v);


        void _debugAddPrimitives(PRIMITIVE_TYPE pt, int num);
    };

    class VideoDriverNull: public IVideoDriver
    {
    public:
        spNativeTexture createTexture();


        void clear(const Color& color) {}
        void begin(const Rect& viewport, const Color* clearColor);
        bool isReady() const {return true;}
        void getViewport(Rect& r) const;
        bool getScissorRect(Rect&) const;
        ShaderProgram*  getShaderProgram() const { return 0; }
        spNativeTexture getRenderTarget() const;

        const VertexDeclaration*    getVertexDeclaration(bvertex_format) const;

        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize) {}
        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize, const unsigned short* indicesData, unsigned int indicesDataSize) {}


        void setUniformInt(const char* id, int v) {}
        void setUniform(const char* id, const Vector4* v, int num) {}
        void setUniform(const char* id, const Vector3* v, int num) {}
        void setUniform(const char* id, const Vector2* v, int num) {}
        void setUniform(const char* id, const Matrix* mat, int num) {}
        void setUniform(const char* id, float val) {}

        void setViewport(const Rect& viewport) {}
        void setScissorRect(const Rect*);
        void setDefaultSettings();
        void setRenderTarget(spNativeTexture);
        void setShaderProgram(ShaderProgram*);
        void setTexture(int sampler, spNativeTexture);
        void setState(STATE, unsigned int value) {}
        void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest) {}
        void setDebugStats(bool enable) {}

        void reset() {}
        void restore() {}

        spNativeTexture _rt;
    };

}
