#pragma once
#include "../oxygine-include.h"
#include "../core/file.h"

namespace oxygine
{
    class ImageData;
    /**
    supported formats = "tga"
    */
    void saveImage(const ImageData& im, const char* path, const char* format = "tga");

    /**
    supported formats = "jpg"
    */
    //void serializeImage(const ImageData &im, file::buffer &bf, const char *format);
}