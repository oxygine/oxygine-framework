#pragma once
#include "../../oxygine-include.h"

#   include "SDL_config.h"

#if _WIN32
#   define GL_GLEXT_PROTOTYPES
#   include "SDL_opengl.h"

#   define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#   define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#   define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#   define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#   define GL_ETC1_RGB8_OES                                        0x8D64


extern "C"
{
//!--pfngl-begin--!
    extern PFNGLSHADERSOURCEPROC _glShaderSource;
    extern PFNGLUSEPROGRAMPROC _glUseProgram;
    extern PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer;
    extern PFNGLACTIVETEXTUREPROC _glActiveTexture;
    extern PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray;
    extern PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray;
    extern PFNGLDELETEPROGRAMPROC _glDeleteProgram;
    extern PFNGLGETSHADERIVPROC _glGetShaderiv;
    extern PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog;
    extern PFNGLCREATESHADERPROC _glCreateShader;
    extern PFNGLCREATEPROGRAMPROC _glCreateProgram;
    extern PFNGLATTACHSHADERPROC _glAttachShader;
    extern PFNGLCOMPILESHADERPROC _glCompileShader;
    extern PFNGLDETACHSHADERPROC _glDetachShader;
    extern PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation;
    extern PFNGLLINKPROGRAMPROC _glLinkProgram;
    extern PFNGLUNIFORM1IPROC _glUniform1i;
    extern PFNGLUNIFORM2FPROC _glUniform2f;
    extern PFNGLUNIFORM2FVPROC _glUniform2fv;
    extern PFNGLUNIFORM3FVPROC _glUniform3fv;
    extern PFNGLUNIFORM3FPROC _glUniform3f;
    extern PFNGLUNIFORM4FVPROC _glUniform4fv;
    extern PFNGLUNIFORM1FPROC _glUniform1f;
    extern PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv;
    extern PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer;
    extern PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers;
    extern PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus;
    extern PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers;
    extern PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
    extern PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D;
    extern PFNGLCOMPRESSEDTEXIMAGE2DPROC _glCompressedTexImage2D;
    extern PFNGLBINDBUFFERPROC _glBindBuffer;
    extern PFNGLGENBUFFERSPROC _glGenBuffers;
    extern PFNGLDRAWBUFFERSPROC _glDrawBuffers;
    extern PFNGLBUFFERDATAPROC _glBufferData;
    extern PFNGLGENTEXTURESPROC _glGenTextures;
    extern PFNGLGETPROGRAMIVPROC _glGetProgramiv;
    extern PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
    extern PFNGLSTENCILOPSEPARATEPROC _glStencilOpSeparate;
    extern PFNGLGENRENDERBUFFERSPROC _glGenRenderbuffers;
    extern PFNGLBINDRENDERBUFFERPROC _glBindRenderbuffer;
    extern PFNGLRENDERBUFFERSTORAGEPROC _glRenderbufferStorage;
    extern PFNGLFRAMEBUFFERRENDERBUFFERPROC _glFramebufferRenderbuffer;
    extern PFNGLDELETERENDERBUFFERSPROC _glDeleteRenderbuffers;
    extern PFNGLDELETESHADERPROC _glDeleteShader;
    extern PFNGLDELETEBUFFERSPROC _glDeleteBuffers;
    extern PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog;
    extern PFNGLBLENDEQUATIONPROC _glBlendEquation;
    extern PFNGLBLENDFUNCSEPARATEPROC _glBlendFuncSeparate;
    extern PFNGLBLENDEQUATIONSEPARATEPROC _glBlendEquationSeparate;
    extern PFNGLGETATTRIBLOCATIONPROC _glGetAttribLocation;
//!--pfngl-end--!
}

#elif __ANDROID__
#   include "GLES2/gl2.h"
#   define GL_GLEXT_PROTOTYPES
#   include "GLES2/gl2ext.h"

#elif __APPLE__
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define GL_ETC1_RGB8_OES                                        0x8D64
#       include <OpenGLES/ES2/gl.h>
#       include <OpenGLES/ES2/glext.h>
#       include <OpenGLES/ES1/gl.h>
#       include <OpenGLES/ES1/glext.h>
#   else
#       define GL_GLEXT_PROTOTYPES
#       include "SDL_opengl.h"

#       define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#       define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#       define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#       define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#       define GL_ETC1_RGB8_OES                                        0x8D64
#   endif
#elif EMSCRIPTEN
#   include "GLES2/gl2.h"
#   define GL_GLEXT_PROTOTYPES
#   include "GLES2/gl2ext.h"
#elif __unix__
#   define GL_GLEXT_PROTOTYPES
#   include "SDL_opengl.h"

#   define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#   define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#   define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#   define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#   define GL_ETC1_RGB8_OES                                        0x8D64
#endif



#if _WIN32
#   define GLPREF _
#else
#   define GLPREF
#endif

#define PASTER(x,y) x ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define DECLARE_GLEXT(y)  EVALUATOR(GLPREF,y)

//!--oxgl-begin--!
#define oxglShaderSource DECLARE_GLEXT(glShaderSource)
#define oxglUseProgram DECLARE_GLEXT(glUseProgram)
#define oxglVertexAttribPointer DECLARE_GLEXT(glVertexAttribPointer)
#define oxglActiveTexture DECLARE_GLEXT(glActiveTexture)
#define oxglEnableVertexAttribArray DECLARE_GLEXT(glEnableVertexAttribArray)
#define oxglDisableVertexAttribArray DECLARE_GLEXT(glDisableVertexAttribArray)
#define oxglDeleteProgram DECLARE_GLEXT(glDeleteProgram)
#define oxglGetShaderiv DECLARE_GLEXT(glGetShaderiv)
#define oxglGetShaderInfoLog DECLARE_GLEXT(glGetShaderInfoLog)
#define oxglCreateShader DECLARE_GLEXT(glCreateShader)
#define oxglCreateProgram DECLARE_GLEXT(glCreateProgram)
#define oxglAttachShader DECLARE_GLEXT(glAttachShader)
#define oxglCompileShader DECLARE_GLEXT(glCompileShader)
#define oxglDetachShader DECLARE_GLEXT(glDetachShader)
#define oxglBindAttribLocation DECLARE_GLEXT(glBindAttribLocation)
#define oxglLinkProgram DECLARE_GLEXT(glLinkProgram)
#define oxglUniform1i DECLARE_GLEXT(glUniform1i)
#define oxglUniform2f DECLARE_GLEXT(glUniform2f)
#define oxglUniform2fv DECLARE_GLEXT(glUniform2fv)
#define oxglUniform3fv DECLARE_GLEXT(glUniform3fv)
#define oxglUniform3f DECLARE_GLEXT(glUniform3f)
#define oxglUniform4fv DECLARE_GLEXT(glUniform4fv)
#define oxglUniform1f DECLARE_GLEXT(glUniform1f)
#define oxglUniformMatrix4fv DECLARE_GLEXT(glUniformMatrix4fv)
#define oxglBindFramebuffer DECLARE_GLEXT(glBindFramebuffer)
#define oxglGenFramebuffers DECLARE_GLEXT(glGenFramebuffers)
#define oxglCheckFramebufferStatus DECLARE_GLEXT(glCheckFramebufferStatus)
#define oxglDeleteFramebuffers DECLARE_GLEXT(glDeleteFramebuffers)
#define oxglGetUniformLocation DECLARE_GLEXT(glGetUniformLocation)
#define oxglFramebufferTexture2D DECLARE_GLEXT(glFramebufferTexture2D)
#define oxglCompressedTexImage2D DECLARE_GLEXT(glCompressedTexImage2D)
#define oxglBindBuffer DECLARE_GLEXT(glBindBuffer)
#define oxglGenBuffers DECLARE_GLEXT(glGenBuffers)
#define oxglBufferData DECLARE_GLEXT(glBufferData)
#define oxglGenTextures DECLARE_GLEXT(glGenTextures)
#define oxglDrawBuffers DECLARE_GLEXT(glDrawBuffers)
#define oxglGetProgramiv DECLARE_GLEXT(glGetProgramiv)
#define oxglGenerateMipmap DECLARE_GLEXT(glGenerateMipmap)
#define oxglStencilOpSeparate DECLARE_GLEXT(glStencilOpSeparate)
#define oxglGenRenderbuffers DECLARE_GLEXT(glGenRenderbuffers)
#define oxglBindRenderbuffer DECLARE_GLEXT(glBindRenderbuffer)
#define oxglRenderbufferStorage DECLARE_GLEXT(glRenderbufferStorage)
#define oxglFramebufferRenderbuffer DECLARE_GLEXT(glFramebufferRenderbuffer)
#define oxglDeleteRenderbuffers DECLARE_GLEXT(glDeleteRenderbuffers)
#define oxglDeleteShader DECLARE_GLEXT(glDeleteShader)
#define oxglDeleteBuffers DECLARE_GLEXT(glDeleteBuffers)
#define oxglGetProgramInfoLog DECLARE_GLEXT(glGetProgramInfoLog)
#define oxglBlendEquation DECLARE_GLEXT(glBlendEquation)
#define oxglBlendFuncSeparate DECLARE_GLEXT(glBlendFuncSeparate)
#define oxglBlendEquationSeparate DECLARE_GLEXT(glBlendEquationSeparate)
#define oxglGetAttribLocation DECLARE_GLEXT(glGetAttribLocation)
//!--oxgl-end--!


//#undef PASTER
//#undef EVALUATOR

namespace oxygine {void checkGLError();}
#define CHECKGL() checkGLError()
//#define CHECKGL() {}

typedef void*  (*myGetProcAdress)(const char*);
/**returns number of missing functions/extensions*/
int initGLExtensions(myGetProcAdress);