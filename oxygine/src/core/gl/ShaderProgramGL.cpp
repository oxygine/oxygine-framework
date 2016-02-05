#include "ShaderProgramGL.h"
#include "VertexDeclarationGL.h"
#include "oxgl.h"

#ifndef __S3E__
#include "SDL.h"
#endif

namespace oxygine
{
    ShaderProgramGL::ShaderProgramGL(): _program(0)
    {

    }

    ShaderProgramGL::~ShaderProgramGL()
    {
        if (_program)
            oxglDeleteProgram(_program);
        CHECKGL();
    }


    void ShaderProgramGL::init(GLuint p)
    {
        _program = p;
    }

    unsigned int ShaderProgramGL::getID() const
    {
        return _program;
    }

    int ShaderProgramGL::getUniformLocation(const char* id) const
    {
        int i = oxglGetUniformLocation(_program, id);
        //if (i == -1)
        CHECKGL();
        return i;
    }

    void printShaderInfoLog(GLuint shader)
    {
        GLint length = 0;
        oxglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length)
        {
            GLint success;
            oxglGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (success != GL_TRUE)
            {

                std::vector<char> bf;
                bf.resize(length);
                oxglGetShaderInfoLog(shader, (int)bf.size(), NULL, &bf.front());

                log::messageln("shader build error: %s", &bf.front());
                OX_ASSERT(!"shader build error");
                exit(1);
            }
        }

        CHECKGL();
    }

    unsigned int ShaderProgramGL::createShader(unsigned int type, const char* data, const char* prepend, const char* append)
    {
        GLuint shader = oxglCreateShader(type);

        const char* sources[16];
        const char** ptr = &sources[0];

        bool gles = false;

#ifdef __S3E__
        gles = true;
#elif   EMSCRIPTEN
        gles = true;
#elif OXYGINE_SDL
        int profile = 0;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

        gles = profile == SDL_GL_CONTEXT_PROFILE_ES;
#else
#endif

        if (gles)
        {
            *ptr = "precision float mediump;";
        }
        else
        {

            log::messageln("not gles version");

            static const char nonGLES[] =
                "#define lowp\n"
                "#define mediump\n"
                "#define highp\n";

            *ptr = nonGLES;
        }
        ptr++;


        if (prepend)
        {
            *ptr = prepend;
            ptr++;
        }

        *ptr = data;
        ptr++;

        if (append)
        {
            *ptr = append;
            ptr++;
        }

        int num = (int)(ptr - sources);
        oxglShaderSource(shader, num, sources, 0);
        oxglCompileShader(shader);
        printShaderInfoLog(shader);

        CHECKGL();

        return shader;
    }


    unsigned int ShaderProgramGL::createProgram(int vs, int fs, const VertexDeclarationGL* decl)
    {
        int p = oxglCreateProgram();
        oxglAttachShader(p, vs);
        oxglAttachShader(p, fs);

        for (int i = 0; i < decl->numElements; ++i)
            oxglBindAttribLocation(p, decl->elements[i].index, decl->elements[i].name);

        oxglLinkProgram(p);

        CHECKGL();

        return p;
    }
}
