#include "oxgl.h"
#include "../ox_debug.h"

#ifdef WIN32
#include "SDL_video.h"

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
	GLAPI void APIENTRY def_glShaderSource(GLuint shader, GLsizei count, const GLchar ** string, const GLint *length)
#else
	GLAPI void APIENTRY def_glShaderSource(GLuint shader, GLsizei count, const GLchar * const* string, const GLint *length)
#endif
	{}
	GLAPI void APIENTRY def_glUseProgram (GLuint program)
	{}
	GLAPI void APIENTRY def_glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
	{}
	GLAPI void APIENTRY def_glActiveTexture (GLenum texture)
	{}
	GLAPI void APIENTRY def_glEnableVertexAttribArray (GLuint index)
	{}
	GLAPI void APIENTRY def_glDisableVertexAttribArray (GLuint index)
	{}
	GLAPI void APIENTRY def_glDeleteProgram (GLuint program)
	{}
	GLAPI void APIENTRY def_glGetShaderiv (GLuint shader, GLenum pname, GLint *params)
	{}
	GLAPI void APIENTRY def_glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
	{}
	GLAPI GLuint APIENTRY def_glCreateShader (GLenum type)
	{return 0;}
	GLAPI GLuint APIENTRY def_glCreateProgram (void)
	{return 0;}
	GLAPI void APIENTRY def_glAttachShader (GLuint program, GLuint shader)
	{}
	GLAPI void APIENTRY def_glCompileShader (GLuint shader)
	{}
	GLAPI void APIENTRY def_glBindAttribLocation (GLuint program, GLuint index, const GLchar *name)
	{}
	GLAPI void APIENTRY def_glLinkProgram (GLuint program)
	{}
	GLAPI void APIENTRY def_glUniform1i (GLint location, GLint v0)
	{}
	GLAPI void APIENTRY def_glUniform2fv (GLint location, GLsizei count, const GLfloat *value)
	{}
	GLAPI void APIENTRY def_glUniform3fv (GLint location, GLsizei count, const GLfloat *value)
	{}
	GLAPI void APIENTRY def_glUniform4fv (GLint location, GLsizei count, const GLfloat *value)
	{}
	GLAPI void APIENTRY def_glUniform1f (GLint location, GLfloat v0)
	{}
	GLAPI void APIENTRY def_glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{}
	GLAPI void APIENTRY def_glBindFramebuffer (GLenum target, GLuint framebuffer)
	{}
	GLAPI void APIENTRY def_glGenFramebuffers (GLsizei n, GLuint *framebuffers)
	{}
	GLAPI GLenum APIENTRY def_glCheckFramebufferStatus (GLenum target)
	{return 0;}
	GLAPI void APIENTRY def_glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers)
	{}
	GLAPI GLint APIENTRY def_glGetUniformLocation (GLuint program, const GLchar *name)
	{return 0;}
	GLAPI void APIENTRY def_glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{}
	GLAPI void APIENTRY def_glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
	{}
	GLAPI void APIENTRY def_glBindBuffer (GLenum target, GLuint buffer)
	{}
	GLAPI void APIENTRY def_glGenBuffers (GLsizei n, GLuint *buffers)
	{}
	GLAPI void APIENTRY def_glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage)
	{}
	GLAPI void APIENTRY def_glDeleteBuffers (GLsizei n, const GLuint *buffers)
	{}

	
	PFNGLSHADERSOURCEPROC _glShaderSource = def_glShaderSource;

#if GL_GLEXT_VERSION == 64
	GLAPI void APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar ** string, const GLint *length)
#else
	GLAPI void APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar * const* string, const GLint *length)
#endif
	{
		_glShaderSource(shader,count,string,length);
	}
	PFNGLUSEPROGRAMPROC _glUseProgram = 0;
	GLAPI void APIENTRY glUseProgram (GLuint program)
	{
		_glUseProgram(program);
	}
	PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer = def_glVertexAttribPointer;
	GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
	{
		_glVertexAttribPointer(index,size,type,normalized,stride,pointer);
	}
	PFNGLACTIVETEXTUREPROC _glActiveTexture = def_glActiveTexture;
	GLAPI void APIENTRY glActiveTexture (GLenum texture)
	{
		_glActiveTexture(texture);
	}
	PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray = def_glEnableVertexAttribArray;
	GLAPI void APIENTRY glEnableVertexAttribArray (GLuint index)
	{
		_glEnableVertexAttribArray(index);
	}
	PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray = def_glDisableVertexAttribArray;
	GLAPI void APIENTRY glDisableVertexAttribArray (GLuint index)
	{
		_glDisableVertexAttribArray(index);
	}
	PFNGLDELETEPROGRAMPROC _glDeleteProgram = def_glDeleteProgram;
	GLAPI void APIENTRY glDeleteProgram (GLuint program)
	{
		_glDeleteProgram(program);
	}
	PFNGLGETSHADERIVPROC _glGetShaderiv = def_glGetShaderiv;
	GLAPI void APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint *params)
	{
		_glGetShaderiv(shader,pname,params);
	}
	PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog = def_glGetShaderInfoLog;
	GLAPI void APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
	{
		_glGetShaderInfoLog(shader,bufSize,length,infoLog);
	}
	PFNGLCREATESHADERPROC _glCreateShader = def_glCreateShader;
	GLAPI GLuint APIENTRY glCreateShader (GLenum type)
	{
		return _glCreateShader(type);
	}
	PFNGLCREATEPROGRAMPROC _glCreateProgram = def_glCreateProgram;
	GLAPI GLuint APIENTRY glCreateProgram (void)
	{
		return _glCreateProgram();
	}
	PFNGLATTACHSHADERPROC _glAttachShader = def_glAttachShader;
	GLAPI void APIENTRY glAttachShader (GLuint program, GLuint shader)
	{
		_glAttachShader(program,shader);
	}
	PFNGLCOMPILESHADERPROC _glCompileShader = def_glCompileShader;
	GLAPI void APIENTRY glCompileShader (GLuint shader)
	{
		_glCompileShader(shader);
	}
	PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation = def_glBindAttribLocation;
	GLAPI void APIENTRY glBindAttribLocation (GLuint program, GLuint index, const GLchar *name)
	{
		_glBindAttribLocation(program,index,name);
	}
	PFNGLLINKPROGRAMPROC _glLinkProgram = def_glLinkProgram;
	GLAPI void APIENTRY glLinkProgram (GLuint program)
	{
		_glLinkProgram(program);
	}
	PFNGLUNIFORM1IPROC _glUniform1i = def_glUniform1i;
	GLAPI void APIENTRY glUniform1i (GLint location, GLint v0)
	{
		_glUniform1i(location,v0);
	}
	PFNGLUNIFORM2FVPROC _glUniform2fv = def_glUniform2fv;
	GLAPI void APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat *value)
	{
		_glUniform2fv(location,count,value);
	}

	PFNGLUNIFORM3FVPROC _glUniform3fv = def_glUniform3fv;
	GLAPI void APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat *value)
	{
		_glUniform3fv(location,count,value);
	}

	PFNGLUNIFORM4FVPROC _glUniform4fv = def_glUniform4fv;
	GLAPI void APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat *value)
	{
		_glUniform4fv(location,count,value);
	}
	PFNGLUNIFORM1FPROC _glUniform1f = def_glUniform1f;
	GLAPI void APIENTRY glUniform1f (GLint location, GLfloat v0)
	{
		_glUniform1f(location,v0);
	}
	PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv = def_glUniformMatrix4fv;
	GLAPI void APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		_glUniformMatrix4fv(location,count,transpose,value);
	}
	PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer = def_glBindFramebuffer;
	GLAPI void APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer)
	{
		_glBindFramebuffer(target,framebuffer);
	}
	PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers = def_glGenFramebuffers;
	GLAPI void APIENTRY glGenFramebuffers (GLsizei n, GLuint *framebuffers)
	{
		_glGenFramebuffers(n,framebuffers);
	}
	PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus = def_glCheckFramebufferStatus;
	GLAPI GLenum APIENTRY glCheckFramebufferStatus (GLenum target)
	{
		return _glCheckFramebufferStatus(target);
	}
	PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers = def_glDeleteFramebuffers;
	GLAPI void APIENTRY glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers)
	{
		_glDeleteFramebuffers(n,framebuffers);
	}
	PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation = def_glGetUniformLocation;
	GLAPI GLint APIENTRY glGetUniformLocation (GLuint program, const GLchar *name)
	{
		return _glGetUniformLocation(program,name);
	}
	PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D = def_glFramebufferTexture2D;
	GLAPI void APIENTRY glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		_glFramebufferTexture2D(target,attachment,textarget,texture,level);
	}
	PFNGLCOMPRESSEDTEXIMAGE2DPROC _glCompressedTexImage2D = def_glCompressedTexImage2D;
	GLAPI void APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
	{
		_glCompressedTexImage2D(target,level,internalformat,width,height,border,imageSize,data);
	}
	PFNGLBINDBUFFERPROC _glBindBuffer = def_glBindBuffer;
	GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer)
	{
		_glBindBuffer(target,buffer);
	}
	PFNGLGENBUFFERSPROC _glGenBuffers = def_glGenBuffers;
	GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers)
	{
		_glGenBuffers(n,buffers);
	}
	PFNGLBUFFERDATAPROC _glBufferData = def_glBufferData;
	GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage)
	{
		_glBufferData(target,size,data,usage);
	}
	PFNGLDELETEBUFFERSPROC _glDeleteBuffers = def_glDeleteBuffers;
	GLAPI void APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers)
	{
		_glDeleteBuffers(n,buffers);
	}



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