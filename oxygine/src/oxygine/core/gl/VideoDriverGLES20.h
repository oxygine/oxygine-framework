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

        void reset() override;
        void restore() override;

        spNativeTexture createTexture() override;

        bool isReady() const  override;

        void begin(const Rect& viewport, const Color* clearColor) override;
        void clear(const Color& color) override;

        ShaderProgram*  getShaderProgram() const override { return _p; }

        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize) override;
        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int verticesDataSize, const unsigned short* indicesData, unsigned int numIndices) override;
        void setDefaultSettings() override;

        void setViewport(const Rect& viewport) override;
        void setShaderProgram(ShaderProgram*) override;
        void setTexture(int sampler, spNativeTexture) override;
        void setUniformInt(const char* id, int v) override;
        void setUniform(const char* id, const Vector4* v, int num) override;
        void setUniform(const char* id, const Vector3* v, int num) override;
        void setUniform(const char* id, const Vector2* v, int num) override;
        void setUniform(const char* id, const Matrix* mat, int num) override;
        void setUniform(const char* id, float val) override;

    protected:
        GLuint _vbo;
        GLuint _ibo;

        int _programID;
        ShaderProgram* _p;
    };
}