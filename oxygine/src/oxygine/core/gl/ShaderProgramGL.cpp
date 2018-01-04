#include "ShaderProgramGL.h"
#include "SDL.h"
#include "VertexDeclarationGL.h"
#include "oxgl.h"

namespace oxygine
{
    ShaderProgramGL::ShaderProgramGL(GLuint p) : _program(p)
    {

    }


    ShaderProgramGL::ShaderProgramGL(GLuint vs, GLuint ps, const VertexDeclarationGL* decl)
    {
        _program = createProgram(vs, ps, decl);
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

    bool ShaderProgramGL::getShaderBuildLog(GLuint shader, std::string& str)
    {
        GLint length = 0;
        GLint success = GL_TRUE;
        oxglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length)
        {
            str.resize(length);
            oxglGetShaderInfoLog(shader, (int)str.size(), NULL, &str[0]);
        }
        else
            str.clear();

        GLint status = GL_TRUE;
        oxglGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        return status == GL_TRUE;
    }


    bool ShaderProgramGL::getProgramBuildLog(GLuint program, std::string& str)
    {
        GLint length = 0;
        GLint success = GL_TRUE;
        oxglGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        if (length)
        {
            str.resize(length);
            oxglGetProgramInfoLog(program, (int)str.size(), NULL, &str[0]);
        }
        else
            str.clear();

        GLint status = GL_TRUE;
        oxglGetProgramiv(program, GL_LINK_STATUS, &status);

        return status == GL_TRUE;

    }

    unsigned int ShaderProgramGL::createShader(unsigned int type, const char* data, const char* prepend, const char* append, error_policy ep)
    {
        GLuint shader = oxglCreateShader(type);

        checkGLError();

        const char* sources[16];
        const char** ptr = &sources[0];

        bool gles = false;


#ifdef OXYGINE_SDL
        int profile = 0;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
        gles = profile == SDL_GL_CONTEXT_PROFILE_ES;
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

#ifdef __ANDROID__
        *ptr = "#define ANDROID 1\n";
        ptr++;
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

        std::string log;
        bool success = getShaderBuildLog(shader, log);

        if (success)
        {
            log::messageln("compiled shader: %s", log.c_str());
        }
        else
        {
            handleErrorPolicy(ep, "can't compile shader: %s", log.c_str());
        }

        checkGLError();

        return shader;
    }


    unsigned int ShaderProgramGL::createProgram(int vs, int fs, const VertexDeclarationGL* decl, bool deleteAttachedShaders)
    {
        int p = oxglCreateProgram();
        oxglAttachShader(p, vs);
        oxglAttachShader(p, fs);

        for (int i = 0; i < decl->numElements; ++i)
            oxglBindAttribLocation(p, decl->elements[i].index, decl->elements[i].name);

        oxglLinkProgram(p);


        std::string log;
        bool success = getProgramBuildLog(p, log);

        if (success)
        {
            //log::messageln("compiled shader: %s", log.c_str());
            oxglDetachShader(p, vs);
            oxglDetachShader(p, fs);
            if (deleteAttachedShaders)
            {
                oxglDeleteShader(vs);
                oxglDeleteShader(fs);
            }
        }
        else
        {
            log::error("can't link gl program: %s", log.c_str());
            oxglDeleteProgram(p);
            p = 0;
        }

        CHECKGL();

        return p;
    }
}
