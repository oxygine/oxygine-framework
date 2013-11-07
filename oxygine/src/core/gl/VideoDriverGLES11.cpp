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

	VideoDriverGLES11::VideoDriverGLES11()
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