#pragma once
#include "../ShaderProgram.h"
#include "oxgl.h"
namespace oxygine
{
    class VertexDeclarationGL;
    class ShaderProgramGL: public ShaderProgram
    {
    public:
        ShaderProgramGL();
        ~ShaderProgramGL();

        void init(GLuint p);

        unsigned int    getID() const;
        int             getUniformLocation(const char* id) const;

        static unsigned int createShader(unsigned int type, const char* data, const char* prepend, const char* append);
        static unsigned int createProgram(int vs, int fs, const VertexDeclarationGL* decl);

    private:
        GLuint _program;
    };


}
