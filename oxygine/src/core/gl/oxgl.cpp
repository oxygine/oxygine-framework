#include "oxgl.h"
#include "../ox_debug.h"

#ifdef WIN32
#include "SDL_video.h"
extern "C"
{
	PFNGLUSEPROGRAMPROC _glUseProgram = 0;
	GLAPI void APIENTRY glUseProgram(GLuint program)
	{
		_glUseProgram(program);
	}
	PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer = 0;
	GLAPI void APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
	{
		_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	}
	PFNGLACTIVETEXTUREPROC _glActiveTexture = 0;
	GLAPI void APIENTRY glActiveTexture(GLenum texture)
	{
		_glActiveTexture(texture);
	}
	PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray = 0;
	GLAPI void APIENTRY glEnableVertexAttribArray(GLuint index)
	{
		_glEnableVertexAttribArray(index);
	}
	PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray = 0;
	GLAPI void APIENTRY glDisableVertexAttribArray(GLuint index)
	{
		_glDisableVertexAttribArray(index);
	}
	PFNGLDELETEPROGRAMPROC _glDeleteProgram = 0;
	GLAPI void APIENTRY glDeleteProgram(GLuint program)
	{
		_glDeleteProgram(program);
	}
	PFNGLGETSHADERIVPROC _glGetShaderiv = 0;
	GLAPI void APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
	{
		_glGetShaderiv(shader, pname, params);
	}
	PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog = 0;
	GLAPI void APIENTRY glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
	{
		_glGetShaderInfoLog(shader, bufSize, length, infoLog);
	}
	PFNGLCREATESHADERPROC _glCreateShader = 0;
	GLAPI GLuint APIENTRY glCreateShader(GLenum type)
	{
		return _glCreateShader(type);
	}
	PFNGLCREATEPROGRAMPROC _glCreateProgram = 0;
	GLAPI GLuint APIENTRY glCreateProgram(void)
	{
		return _glCreateProgram();
	}
	PFNGLATTACHSHADERPROC _glAttachShader = 0;
	GLAPI void APIENTRY glAttachShader(GLuint program, GLuint shader)
	{
		_glAttachShader(program, shader);
	}
	PFNGLSHADERSOURCEPROC _glShaderSource = 0;
	GLAPI void APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar * const* string, const GLint *length)
	{
		_glShaderSource(shader, count, string, length);
	}
	PFNGLCOMPILESHADERPROC _glCompileShader = 0;
	GLAPI void APIENTRY glCompileShader(GLuint shader)
	{
		_glCompileShader(shader);
	}
	PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation = 0;
	GLAPI void APIENTRY glBindAttribLocation(GLuint program, GLuint index, const GLchar *name)
	{
		_glBindAttribLocation(program, index, name);
	}
	PFNGLLINKPROGRAMPROC _glLinkProgram = 0;
	GLAPI void APIENTRY glLinkProgram(GLuint program)
	{
		_glLinkProgram(program);
	}
	PFNGLUNIFORM1IPROC _glUniform1i = 0;
	GLAPI void APIENTRY glUniform1i(GLint location, GLint v0)
	{
		_glUniform1i(location, v0);
	}
	PFNGLUNIFORM4FVPROC _glUniform4fv = 0;
	GLAPI void APIENTRY glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
	{
		_glUniform4fv(location, count, value);
	}
	PFNGLUNIFORM1FPROC _glUniform1f = 0;
	GLAPI void APIENTRY glUniform1f(GLint location, GLfloat v0)
	{
		_glUniform1f(location, v0);
	}
	PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv = 0;
	GLAPI void APIENTRY glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		_glUniformMatrix4fv(location, count, transpose, value);
	}
	PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer = 0;
	GLAPI void APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
	{
		_glBindFramebuffer(target, framebuffer);
	}
	PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers = 0;
	GLAPI void APIENTRY glGenFramebuffers(GLsizei n, GLuint *framebuffers)
	{
		_glGenFramebuffers(n, framebuffers);
	}
	PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus = 0;
	GLAPI GLenum APIENTRY glCheckFramebufferStatus(GLenum target)
	{
		return _glCheckFramebufferStatus(target);
	}
	PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers = 0;
	GLAPI void APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
	{
		_glDeleteFramebuffers(n, framebuffers);
	}
	PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation = 0;
	GLAPI GLint APIENTRY glGetUniformLocation(GLuint program, const GLchar *name)
	{
		return _glGetUniformLocation(program, name);
	}
	PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D = 0;
	GLAPI void APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		_glFramebufferTexture2D(target, attachment, textarget, texture, level);
	}
	PFNGLCOMPRESSEDTEXIMAGE2DPROC _glCompressedTexImage2D = 0;
	GLAPI void APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
	{
		_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	}
}

void initGLExtensions(myGetProcAdress func)
{
    if (_glUseProgram)
        return;

	_glUseProgram = (PFNGLUSEPROGRAMPROC)func("glUseProgram");
	_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)func("glVertexAttribPointer");
	_glActiveTexture = (PFNGLACTIVETEXTUREPROC)func("glActiveTexture");
	_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)func("glEnableVertexAttribArray");
	_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)func("glDisableVertexAttribArray");
	_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)func("glDeleteProgram");
	_glGetShaderiv = (PFNGLGETSHADERIVPROC)func("glGetShaderiv");
	_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)func("glGetShaderInfoLog");
	_glCreateShader = (PFNGLCREATESHADERPROC)func("glCreateShader");
	_glCreateProgram = (PFNGLCREATEPROGRAMPROC)func("glCreateProgram");
	_glAttachShader = (PFNGLATTACHSHADERPROC)func("glAttachShader");
	_glShaderSource = (PFNGLSHADERSOURCEPROC)func("glShaderSource");
	_glCompileShader = (PFNGLCOMPILESHADERPROC)func("glCompileShader");
	_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)func("glBindAttribLocation");
	_glLinkProgram = (PFNGLLINKPROGRAMPROC)func("glLinkProgram");
	_glUniform1i = (PFNGLUNIFORM1IPROC)func("glUniform1i");
	_glUniform4fv = (PFNGLUNIFORM4FVPROC)func("glUniform4fv");
	_glUniform1f = (PFNGLUNIFORM1FPROC)func("glUniform1f");
	_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)func("glUniformMatrix4fv");
	_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)func("glBindFramebuffer");
	_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)func("glGenFramebuffers");
	_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)func("glCheckFramebufferStatus");
	_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)func("glDeleteFramebuffers");
	_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)func("glGetUniformLocation");
	_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)func("glFramebufferTexture2D");
	_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)func("glCompressedTexImage2D");
}
#else
void initGLExtensions(myGetProcAdress func)
{
}
#endif
