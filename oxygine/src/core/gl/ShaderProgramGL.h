#pragma once
#include "../ShaderProgram.h"
#include "oxgl.h"
namespace oxygine
{
    class VertexDeclarationGL;
    class ShaderProgramGL: public ShaderProgram
    {
    public:
        ShaderProgramGL(GLuint program);
        ShaderProgramGL(GLuint vs, GLuint fs, const VertexDeclarationGL* decl);
        ~ShaderProgramGL();

        unsigned int    getID() const;
        int             getUniformLocation(const char* id) const;

        static unsigned int createShader(unsigned int type, const char* data,
                                         const char* prepend, const char* append, error_policy ep = ep_show_error);
        static unsigned int createProgram(int vs, int fs, const VertexDeclarationGL* decl);
        static bool getShaderBuildLog(GLuint shader, std::string& str);

    private:
        GLuint _program;
    };


}
