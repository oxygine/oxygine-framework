#include "VideoDriverGLES11.h"
#include "NativeTextureGLES.h"
#include "oxgl.h"
#include "math/Color.h"


namespace oxygine
{
	spNativeTexture VideoDriverGLES11::createTexture()
	{
		return new NativeTextureGLES();
	}


	void VideoDriverGLES11::drawBatch(const batch &b)
	{
		const VertexDeclarationGL *decl = static_cast<const VertexDeclarationGL*>(b.vdecl);

		int count = b.vertices.size() / decl->size;
		int indices = (count * 3)/2;
		const unsigned char *p = &b.vertices[0];


		unsigned int id = (unsigned int)b.base->getHandle();

		if (_blend != b.blend)
		{
			_blend = b.blend;

			switch(b.blend)
			{
			case blend_disabled:
				glDisable(GL_BLEND);
				break;

			case blend_premultiplied_alpha:
				glEnable     (GL_BLEND);
				glBlendFunc  (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				break;

			case blend_alpha:
				glEnable     (GL_BLEND);
				glBlendFunc  (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

			case blend_add:
				glEnable     (GL_BLEND);
				glBlendFunc  (GL_ONE, GL_ONE);
				break;
				/*
			case blend_xor:
				glEnable     (GL_BLEND);
				glBlendFunc  (GL_ZERO, GL_ONE_MINUS_DST_COLOR);
				break;
				*/
			default:
				//OX_ASSERT(0);
				break;
			}				
		}			

		glBindTexture(GL_TEXTURE_2D, id);

		/*
		const VertexDeclarationGL::Element *el = decl->elements;
		for (int i = 0; i < decl->numElements; ++i)		
		{
			glEnableVertexAttribArray(el->index);
			glVertexAttribPointer(el->index, el->size, el->elemType, el->normalized, decl->size, p + el->offset);
			el++;
		}
		*/


		OX_ASSERT(decl->bformat == VERTEX_PCT2);
		glVertexPointer(3, GL_FLOAT, decl->size, p + decl->elements[0].offset);
		glEnableClientState(GL_VERTEX_ARRAY);

		glColorPointer(4, GL_UNSIGNED_BYTE, decl->size, p + decl->elements[1].offset);
		glEnableClientState(GL_COLOR_ARRAY); 

		glTexCoordPointer(2, GL_FLOAT, decl->size, p + decl->elements[2].offset);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);			

		//glDrawArrays(GL_TRIANGLES, 0, count);

		if (indices <= (int)_indices8.size())
			glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_BYTE, &_indices8[0]);
		else
			glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, &_indices16[0]);
	}

	void VideoDriverGLES11::setDefaultSettings()
	{
		glDisable(GL_ALPHA_TEST);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glShadeModel(GL_SMOOTH);

		glCullFace(GL_FRONT_AND_BACK);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
	}

	VideoDriverGLES11::VideoDriverGLES11():_blend(blend_default)
	{

	}

	VideoDriverGLES11::~VideoDriverGLES11()
	{

	}

	void VideoDriverGLES11::begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *clearColor)
	{
		_begin(viewport, clearColor);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(proj.ml);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(view.ml);
	}
}