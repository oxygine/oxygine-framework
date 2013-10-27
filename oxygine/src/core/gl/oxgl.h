#pragma once
#include "oxygine_include.h"


#if __S3E__
	#include "IwImage.h"
	#include "IwGL.h"
	#include "GLES/gl.h"
	#define GL_GLEXT_PROTOTYPES 
	#include "GLES/glext.h"
	#include <gles/egl.h>
#elif WIN32
	#include "glew.h"

	#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
	#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
	#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
	#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

	#define GL_ETC1_RGB8_OES                                        0x8D64

#elif __ANDROID__
	#include "GLES2/gl2.h"
	#define GL_GLEXT_PROTOTYPES 
	#include "GLES2/gl2ext.h"

	#define glGenFramebuffers			glGenFramebuffers
	#define glBindFramebuffer			glBindFramebuffer
	#define glFramebufferTexture2D		glFramebufferTexture2D
	#define glDeleteFramebuffers		glDeleteFramebuffers
	#define glCheckFramebufferStatus	glCheckFramebufferStatus

	/*
	#define GL_FRAMEBUFFER				GL_FRAMEBUFFER
	#define GL_FRAMEBUFFER_BINDING		GL_FRAMEBUFFER_BINDING
	#define GL_COLOR_ATTACHMENT0		GL_COLOR_ATTACHMENT0
	#define GL_FRAMEBUFFER_COMPLETE		GL_FRAMEBUFFER_COMPLETE
	*/
#elif __APPLE__
    #define GL_ETC1_RGB8_OES                                        0x8D64
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#elif __unix__
	#define GL_GLEXT_PROTOTYPES
	#include "SDL_opengl.h"

	#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
	#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
	#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
	#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

	#define GL_ETC1_RGB8_OES                                        0x8D64


#endif

	void initGLExtensions();
