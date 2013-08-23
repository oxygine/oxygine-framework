#pragma once
#include "core/files_io.h"
namespace oxygine
{
	class ImageData;
	/**
	ImageData should be TF_R8G8B8A8
	supported formats = "jpg", "png", "tga"
	*/
	void saveImage(const ImageData &im, const char *path, const char *format);

	/**
	supported formats = "jpg"
	*/
	//void serializeImage(const ImageData &im, file::buffer &bf, const char *format);
}