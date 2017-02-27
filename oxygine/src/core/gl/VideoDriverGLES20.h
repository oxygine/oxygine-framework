#pragma once
#include "oxygine-include.h"
#include "VideoDriverGL.h"
#include "core/Restorable.h"
#include "../ShaderProgram.h"
#include "../file.h"

namespace oxygine
{
    class VideoDriverGLES20: public VideoDriverGL
    {
    public:
        VideoDriverGLES20();
        ~VideoDriverGLES20();

        void reset();
        void restore();

        spNativeTexture createTexture();

        bool isReady() const;

        void begin(const Rect& viewport, const Color* clearColor);
        void clear(const Color& color);

        ShaderProgram*  getShaderProgram() const override { return _p; }

        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize);
        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize, const unsigned short* indicesData, unsigned int numIndices);
        void setDefaultSettings();

        void setViewport(const Rect& viewport);
        void setShaderProgram(ShaderProgram*);
        void setTexture(int sampler, spNativeTexture);
        void setUniformInt(const char* id, int v);
        void setUniform(const char* id, const Vector4* v, int num);
        void setUniform(const char* id, const Vector3* v, int num);
        void setUniform(const char* id, const Vector2* v, int num);
        void setUniform(const char* id, const Matrix* mat);
        void setUniform(const char* id, float val);

    protected:
        GLuint _vbo;
        GLuint _ibo;

        int _programID;
        ShaderProgram* _p;
    };
}