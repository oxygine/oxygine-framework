#include "VideoDriverGL.h"
#include "NativeTextureGLES.h"
#include "oxgl.h"
#include "math/Color.h"
#include "utils/stringUtils.h"

namespace oxygine
{
	VideoDriverGL::VideoDriverGL():_prevFBO(0)
	{

	}

	int VideoDriverGL::getMaxVertices() const
	{
		int m = _indices16.size()/3 * 2;
		return m;
	}

	bool VideoDriverGL::getScissorRect(Rect &r) const
	{
		GLboolean scrTest = glIsEnabled(GL_SCISSOR_TEST);

		GLint box[4];
		glGetIntegerv(GL_SCISSOR_BOX, box);
		r = Rect(box[0], box[1], box[2], box[3]);

		return scrTest ? true : false;	
	}

	const VertexDeclarationGL* VideoDriverGL::getVertexDeclaration(bvertex_format fmt) const
	{
		return _vdeclarations.get(fmt);
	}

	void VideoDriverGL::getViewport(Rect &r) const
	{
		GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);

		r = Rect(vp[0], vp[1], vp[2], vp[3]);;
	}

	void VideoDriverGL::setScissorRect(const Rect *rect)
	{
		if (rect)
		{
			glEnable(GL_SCISSOR_TEST);
			glScissor(rect->getX(), rect->getY(), rect->getWidth(), rect->getHeight());
		}
		else
			glDisable(GL_SCISSOR_TEST);
	}

	void VideoDriverGL::setRenderTarget(spNativeTexture rt)
	{
		if (!rt)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _prevFBO);
			return;
		}

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_prevFBO);

		NativeTextureGLES *gl = safeCast<NativeTextureGLES*>(rt.get());		
		glBindFramebuffer(GL_FRAMEBUFFER, gl->getFboID());
	}

	void VideoDriverGL::_begin(const Rect &viewport, const Color *clearColor)
	{
		glViewport(viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight());
		glDisable(GL_SCISSOR_TEST);
		if (clearColor)
		{
			glClearColor(clearColor->getRedF(), clearColor->getGreenF(), clearColor->getBlueF(), clearColor->getAlphaF());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}

	/*
	void VertexDeclarationGL::init(bvertex_format fmt)
	{
		bformat = fmt;
		int i = 0;
		int offset = 0;
		VertextElement *dest = elements;
		if (fmt & VERTEX_POSITION)
		{
			strcpy(dest->name, "position");
			dest->elemType = GL_FLOAT;
			dest->size = 3;
			dest->offset = offset;
			dest->index = i;
			dest->normalized = false;

			offset += sizeof(float) * 3;
			++i;
			++dest;
		}

		if (fmt & VERTEX_COLOR)
		{
			strcpy(dest->name, "color");
			dest->elemType = GL_UNSIGNED_BYTE;
			dest->size = 4;
			dest->offset = offset;
			dest->index = i;
			dest->normalized = true;

			offset += 4;
			++i;
			++dest;
		}

		int numTexCoords = numTextureCoordinates(fmt);
		for (int j = 0; j < numTexCoords; ++j )
		{
			int coordSize = texCoordSize(j, fmt);
			if (j == 0)
				strcpy(dest->name, "uv");
			else
				safe_sprintf(dest->name, "uv%d", j + 1);

			dest->elemType = GL_FLOAT;
			dest->size = coordSize;
			dest->offset = offset;
			dest->index = i;
			dest->normalized = true;

			offset += sizeof(float) * coordSize;
			++i;
			++dest;
		}

		int userSize = userDataSize(fmt);
		if (userSize > 0)
		{
			dest->elemType = GL_FLOAT;
			dest->size = userSize;
			dest->offset = offset;
			dest->index = i;
			dest->normalized = false;

			offset += sizeof(float) * userSize;
			++i;
			++dest;
		}

		numElements = i;
		size = offset;
	}

	VertexDeclarationGL declarations[8];

	const VertexDeclarationGL *VertexDeclarationGL::getDeclaration(bvertex_format bformat)
	{
		for (int i = 0; i < 8; ++i)
		{
			VertexDeclarationGL &decl = declarations[i];
			if (decl.bformat == 0)
			{
				decl.init(bformat);
			}
			if (decl.bformat == bformat)
				return &decl;			
		}

		OX_ASSERT(0);
		return 0;
	}
	*/
}