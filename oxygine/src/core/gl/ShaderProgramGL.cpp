#include "ShaderProgramGL.h"
#include "VertexDeclarationGL.h"
#include "oxgl.h"

#ifndef __S3E__
#include "SDL.h"
#endif

namespace oxygine
{
    ShaderProgramGL::ShaderProgramGL(GLuint p) : _program(p)
    {

    }

    ShaderProgramGL::~ShaderProgramGL()
    {
        if (_program)
            oxglDeleteProgram(_program);
        CHECKGL();
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

    GLint ShaderProgramGL::getShaderInfoLog(GLuint shader, std::vector<char>& log)
    {
        GLint length = 0;
        GLint success = GL_TRUE;
        oxglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length)
        {
            oxglGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (success != GL_TRUE)
            {
                log.resize(length);
                oxglGetShaderInfoLog(shader, (int)log.size(), NULL, &log.front());
            }
        }

        return success;
    }

    unsigned int ShaderProgramGL::createShader(unsigned int type, const char* data, const char* prepend, const char* append, error_policy ep)
    {
        GLuint shader = oxglCreateShader(type);

        const char* sources[16];
        const char** ptr = &sources[0];

        bool gles = false;

#ifdef __S3E__
        gles = true;
#elif OXYGINE_SDL
        int profile = 0;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

        gles = profile == SDL_GL_CONTEXT_PROFILE_ES;
#else
#endif


#ifndef   EMSCRIPTEN
        if (!gles)
        {
            //log::messageln("not gles version");

            static const char nonGLES[] =
                "#define lowp\n"
                "#define mediump\n"
                "#define highp\n";

            *ptr = nonGLES;
            ptr++;
        }
#endif


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
        
        if(ep == ep_show_error || ep == ep_show_warning)
        {
            std::vector<char> log;
            GLint status = getShaderInfoLog(shader, log);
            if(status != GL_TRUE)
            {
                log::messageln("shader build error: %s", &log.front());
                if(ep == ep_show_error)
                {
                    OX_ASSERT(!"shader build error");
                    exit(1);
                }
            }

            CHECKGL();
        }

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
