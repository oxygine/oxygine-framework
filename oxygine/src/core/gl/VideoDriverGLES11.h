#pragma once
#include "VideoDriverGL.h"

namespace oxygine
{
	class VideoDriverGLES11: public VideoDriverGL
	{
	public:
		VideoDriverGLES11();
		~VideoDriverGLES11();

		spNativeTexture createTexture();

		void begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *clearColor);
		
		void drawBatch(const batch &b);
		void setDefaultSettings();

	private:
		blend_mode _blend;
	};
}