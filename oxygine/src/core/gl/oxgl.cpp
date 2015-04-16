#include "oxgl.h"
#include "../ox_debug.h"
#include <stdlib.h>

#ifdef _WIN32
//#include "SDL_video.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4273)
#endif

//fix compile time error with latest SDL
#undef GLAPI
#define GLAPI

#define GETFUNC(var, df, tp, name) var = (tp)func(name); if (!var) {var = df; missing++;}

extern "C"
{
#if GL_GLEXT_VERSION == 64
    GLAPI void APIENTRY def_glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
#else
    GLAPI void APIENTRY def_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length)
#endif
    {}
    GLAPI void APIENTRY def_glUseProgram(GLuint program)
    {}
    GLAPI void APIENTRY def_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
    {}
    GLAPI void APIENTRY def_glActiveTexture(GLenum texture)
    {}
    GLAPI void APIENTRY def_glEnableVertexAttribArray(GLuint index)
    {}
    GLAPI void APIENTRY def_glDisableVertexAttribArray(GLuint index)
    {}
    GLAPI void APIENTRY def_glDeleteProgram(GLuint program)
    {}
    GLAPI void APIENTRY def_glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
    {}
    GLAPI void APIENTRY def_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
    {}
    GLAPI GLuint APIENTRY def_glCreateShader(GLenum type)
    {return 0;}
    GLAPI GLuint APIENTRY def_glCreateProgram(void)
    {return 0;}
    GLAPI void APIENTRY def_glAttachShader(GLuint program, GLuint shader)
    {}
    GLAPI void APIENTRY def_glCompileShader(GLuint shader)
    {}
    GLAPI void APIENTRY def_glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)
    {}
    GLAPI void APIENTRY def_glLinkProgram(GLuint program)
    {}
    GLAPI void APIENTRY def_glUniform1i(GLint location, GLint v0)
    {}
    GLAPI void APIENTRY def_glUniform2fv(GLint location, GLsizei count, const GLfloat* value)
    {}
    GLAPI void APIENTRY def_glUniform3fv(GLint location, GLsizei count, const GLfloat* value)
    {}
    GLAPI void APIENTRY def_glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
    {}
    GLAPI void APIENTRY def_glUniform1f(GLint location, GLfloat v0)
    {}
    GLAPI void APIENTRY def_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
    {}
    GLAPI void APIENTRY def_glBindFramebuffer(GLenum target, GLuint framebuffer)
    {}
    GLAPI void APIENTRY def_glGenFramebuffers(GLsizei n, GLuint* framebuffers)
    {}
    GLAPI GLenum APIENTRY def_glCheckFramebufferStatus(GLenum target)
    {return 0;}
    GLAPI void APIENTRY def_glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
    {}
    GLAPI GLint APIENTRY def_glGetUniformLocation(GLuint program, const GLchar* name)
    {return 0;}
    GLAPI void APIENTRY def_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
    {}
    GLAPI void APIENTRY def_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
    {}
    GLAPI void APIENTRY def_glBindBuffer(GLenum target, GLuint buffer)
    {}
    GLAPI void APIENTRY def_glGenBuffers(GLsizei n, GLuint* buffers)
    {}
    GLAPI void APIENTRY def_glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
    {}
    GLAPI void APIENTRY def_glDeleteBuffers(GLsizei n, const GLuint* buffers)
    {}


    PFNGLSHADERSOURCEPROC _glShaderSource = 0;
    PFNGLUSEPROGRAMPROC _glUseProgram = 0;
    PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer = 0;
    PFNGLACTIVETEXTUREPROC _glActiveTexture = 0;
    PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray = 0;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray = 0;
    PFNGLDELETEPROGRAMPROC _glDeleteProgram = 0;
    PFNGLGETSHADERIVPROC _glGetShaderiv = 0;
    PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog = 0;
    PFNGLCREATESHADERPROC _glCreateShader = 0;
    PFNGLCREATEPROGRAMPROC _glCreateProgram = 0;
    PFNGLATTACHSHADERPROC _glAttachShader = 0;
    PFNGLCOMPILESHADERPROC _glCompileShader = 0;
    PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation = 0;
    PFNGLLINKPROGRAMPROC _glLinkProgram = 0;
    PFNGLUNIFORM1IPROC _glUniform1i = 0;
    PFNGLUNIFORM2FVPROC _glUniform2fv = 0;
    PFNGLUNIFORM3FVPROC _glUniform3fv = 0;
    PFNGLUNIFORM4FVPROC _glUniform4fv = 0;
    PFNGLUNIFORM1FPROC _glUniform1f = 0;
    PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv = 0;
    PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer = 0;
    PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers = 0;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus = 0;
    PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers = 0;
    PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation = 0;
    PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D = 0;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC _glCompressedTexImage2D = 0;
    PFNGLBINDBUFFERPROC _glBindBuffer = 0;
    PFNGLGENBUFFERSPROC _glGenBuffers = 0;
    PFNGLBUFFERDATAPROC _glBufferData = 0;
    PFNGLDELETEBUFFERSPROC _glDeleteBuffers = 0;
}

int initGLExtensions(myGetProcAdress func)
{
    if (_glUseProgram)
        return 0;

    int missing = 0;

    GETFUNC(_glShaderSource, def_glShaderSource, PFNGLSHADERSOURCEPROC, "glShaderSource");
    GETFUNC(_glUseProgram, def_glUseProgram, PFNGLUSEPROGRAMPROC, "glUseProgram");
    GETFUNC(_glVertexAttribPointer, def_glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC, "glVertexAttribPointer");
    GETFUNC(_glActiveTexture, def_glActiveTexture, PFNGLACTIVETEXTUREPROC, "glActiveTexture");
    GETFUNC(_glEnableVertexAttribArray, def_glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC, "glEnableVertexAttribArray");
    GETFUNC(_glDisableVertexAttribArray, def_glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC, "glDisableVertexAttribArray");
    GETFUNC(_glDeleteProgram, def_glDeleteProgram, PFNGLDELETEPROGRAMPROC, "glDeleteProgram");
    GETFUNC(_glGetShaderiv, def_glGetShaderiv, PFNGLGETSHADERIVPROC, "glGetShaderiv");
    GETFUNC(_glGetShaderInfoLog, def_glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC, "glGetShaderInfoLog");
    GETFUNC(_glCreateShader, def_glCreateShader, PFNGLCREATESHADERPROC, "glCreateShader");
    GETFUNC(_glCreateProgram, def_glCreateProgram, PFNGLCREATEPROGRAMPROC, "glCreateProgram");
    GETFUNC(_glAttachShader, def_glAttachShader, PFNGLATTACHSHADERPROC, "glAttachShader");
    GETFUNC(_glCompileShader, def_glCompileShader, PFNGLCOMPILESHADERPROC, "glCompileShader");
    GETFUNC(_glBindAttribLocation, def_glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC, "glBindAttribLocation");
    GETFUNC(_glLinkProgram, def_glLinkProgram, PFNGLLINKPROGRAMPROC, "glLinkProgram");
    GETFUNC(_glUniform1i, def_glUniform1i, PFNGLUNIFORM1IPROC, "glUniform1i");
    GETFUNC(_glUniform2fv, def_glUniform2fv, PFNGLUNIFORM2FVPROC, "glUniform2fv");
    GETFUNC(_glUniform3fv, def_glUniform3fv, PFNGLUNIFORM3FVPROC, "glUniform3fv");
    GETFUNC(_glUniform4fv, def_glUniform4fv, PFNGLUNIFORM4FVPROC, "glUniform4fv");
    GETFUNC(_glUniform1f, def_glUniform1f, PFNGLUNIFORM1FPROC, "glUniform1f");
    GETFUNC(_glUniformMatrix4fv, def_glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC, "glUniformMatrix4fv");
    GETFUNC(_glBindFramebuffer, def_glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC, "glBindFramebuffer");
    GETFUNC(_glGenFramebuffers, def_glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC, "glGenFramebuffers");
    GETFUNC(_glCheckFramebufferStatus, def_glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC, "glCheckFramebufferStatus");
    GETFUNC(_glDeleteFramebuffers, def_glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC, "glDeleteFramebuffers");
    GETFUNC(_glGetUniformLocation, def_glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC, "glGetUniformLocation");
    GETFUNC(_glFramebufferTexture2D, def_glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC, "glFramebufferTexture2D");
    GETFUNC(_glCompressedTexImage2D, def_glCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPROC, "glCompressedTexImage2D");
    GETFUNC(_glBindBuffer, def_glBindBuffer, PFNGLBINDBUFFERPROC, "glBindBuffer");
    GETFUNC(_glGenBuffers, def_glGenBuffers, PFNGLGENBUFFERSPROC, "glGenBuffers");
    GETFUNC(_glBufferData, def_glBufferData, PFNGLBUFFERDATAPROC, "glBufferData");
    GETFUNC(_glDeleteBuffers, def_glDeleteBuffers, PFNGLDELETEBUFFERSPROC, "glDeleteBuffers");

    return missing;
}
#else
int initGLExtensions(myGetProcAdress func)
{
    return 0;
}
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


namespace oxygine
{
    void checkGLError()
    {
        int gl_error = glGetError();
        if (gl_error != GL_NO_ERROR)
        {
            log::error("OpenGL error: %#x\n", gl_error);
            if (gl_error == GL_OUT_OF_MEMORY)
            {
                exit(0);
            }
        }
    }
}
