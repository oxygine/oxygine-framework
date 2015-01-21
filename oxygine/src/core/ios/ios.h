#pragma once
#include <string>
#include "ImageData.h"

namespace oxygine
{
	namespace file
	{
		std::string getSupportFolder();
	}
    
    class MemoryTexture;
    
    bool nsImageLoad(MemoryTexture &mt, void * pData, int nDatalen, bool premultiplied, TextureFormat format);
}