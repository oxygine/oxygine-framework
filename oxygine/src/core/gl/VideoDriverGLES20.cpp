#include "VideoDriverGLES20.h"
#include "NativeTextureGLES.h"
#include "ShaderProgramGL.h"
#include "oxgl.h"
#include "math/Color.h"
#include "utils/stringUtils.h"
#include "../vertex.h"
#include "../file.h"
#include "../log.h"
#include "../ZipFileSystem.h"
#include "../system_data.h"

#if OXYGINE_SDL
#include "SDL_config.h"
#endif


#ifdef __ANDROID__
#include "GLES2/gl2.h"
#endif

namespace oxygine
{    
	VideoDriverGLES20::VideoDriverGLES20():_program(0)
	{
	}

	VideoDriverGLES20::~VideoDriverGLES20()
	{
		glUseProgram(0);
        CHECKGL();
	}

	void VideoDriverGLES20::restore()
	{
		//_us.restore();
		setDefaultSettings();
	}

	bool VideoDriverGLES20::isReady() const
	{
		return true;
	}

	spNativeTexture VideoDriverGLES20::createTexture()
	{
		return new NativeTextureGLES;
	}

	void VideoDriverGLES20::reset()
	{
		//_currentProgram = 0;
	}

	void VideoDriverGLES20::updateConstants()
	{
		//_currentProgram->setUniform("mat", &_matrixVP);
	}


	void VideoDriverGLES20::begin(const Rect &viewport, const Color *clearColor)
	{
		_begin(viewport, clearColor);
	}

	void VideoDriverGLES20::setShaderProgram(ShaderProgram* prog_)
	{
        CHECKGL();
		ShaderProgramGL* prog = safeCast<ShaderProgramGL*>(prog_);
		unsigned int id = prog->getID();
		glUseProgram(id);
		_program = id;
        CHECKGL();
	}

	void VideoDriverGLES20::setTexture(int sampler, spNativeTexture t)
	{
		glActiveTexture(GL_TEXTURE0 + sampler);
		if (t)
			glBindTexture(GL_TEXTURE_2D, (GLuint)((size_t)t->getHandle()));	
		else
			glBindTexture(GL_TEXTURE_2D, 0);
        CHECKGL();
	}

	

	void VideoDriverGLES20::draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl_, const void *vdata, unsigned int size)
	{
		const VertexDeclarationGL *decl = static_cast<const VertexDeclarationGL*>(decl_);

		const unsigned char *verticesData = (const unsigned char *)vdata;

		const VertexDeclarationGL::Element *el = decl->elements;
		for (int i = 0; i < decl->numElements; ++i)		
		{	
			glEnableVertexAttribArray(el->index);
			glVertexAttribPointer(el->index, el->size, el->elemType, el->normalized, decl->size, verticesData + el->offset);			
			el++;
		}

		size_t primitives = size / decl->size;
		glDrawArrays(getPT(pt), 0, primitives);

		el = decl->elements;
		for (int i = 0; i < decl->numElements; ++i)		
		{			
			glDisableVertexAttribArray(el->index);
			el++;
		}


#if OXYGINE_TRACE_VIDEO_STATS
		_debugAddPrimitives(pt, primitives);
#endif
        
        CHECKGL();
	}

	void VideoDriverGLES20::draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl_, const void *vdata, unsigned int numVertices, const void *indicesData, unsigned int numIndices, bool indicesShortType)
	{
		const VertexDeclarationGL *decl = static_cast<const VertexDeclarationGL*>(decl_);

		const unsigned char *verticesData = (const unsigned char *)vdata;

		const VertexDeclarationGL::Element *el = decl->elements;
		for (int i = 0; i < decl->numElements; ++i)		
		{	
			glEnableVertexAttribArray(el->index);
			glVertexAttribPointer(el->index, el->size, el->elemType, el->normalized, decl->size, verticesData + el->offset);			
			el++;
		}

		glDrawElements(getPT(pt), numIndices, indicesShortType ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, indicesData);

		el = decl->elements;
		for (int i = 0; i < decl->numElements; ++i)		
		{			
			glDisableVertexAttribArray(el->index);
			el++;
		}


#if OXYGINE_TRACE_VIDEO_STATS
		_debugAddPrimitives(pt, numIndices);
#endif
        CHECKGL();
	}

	void VideoDriverGLES20::setDefaultSettings()
	{
		//glEnable(GL_TEXTURE_2D);

		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glCullFace(GL_FRONT_AND_BACK);
        
        CHECKGL();
		//setProgram(_us.getShaderProgram(0)->program);
	}

	void VideoDriverGLES20::setUniformInt(const char *id, int v)
	{
		int location = glGetUniformLocation(_program, id);
		if (location == -1)
			return;
		glUniform1i(location, v);
		CHECKGL();
	}



	void VideoDriverGLES20::setUniform(const char *id, const Vector4 *v, int num)
	{
		int p = glGetUniformLocation(_program, id);
		if (p == -1)
			return;
		glUniform4fv(p, num, v->m);
		CHECKGL();
	}

	void VideoDriverGLES20::setUniform(const char *id, const Matrix *mat)
	{
		int p = glGetUniformLocation(_program, id);
		if (p == -1)
			return;
		glUniformMatrix4fv(p, 1, GL_FALSE, mat->ml);
		CHECKGL();
	}

	void VideoDriverGLES20::setUniform(const char *id, float val)
	{
		int p = glGetUniformLocation(_program, id);
		if (p == -1)
			return;
		glUniform1f(p, val);
		CHECKGL();
	}

}