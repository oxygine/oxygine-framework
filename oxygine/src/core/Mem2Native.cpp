#include "Mem2Native.h"
#include "ImageDataOperations.h"

namespace oxygine
{
	const int RECT_SIZE = 4096;

	Mem2Native::Mem2Native():_prev(0, 0), _size(0)
	{
	}

	Mem2Native::~Mem2Native()
	{
	}

	void Mem2Native::push(spMemoryTexture src, spNativeTexture dest)
	{	
		src->addRef();
		dest->addRef();
		_messages.send(0, src.get(), dest.get());
		//_messages.post(0, src.get(), dest.get());
	}
	
	bool Mem2Native::isEmpty()
	{
		return _prev == Point(0,0) && _messages.empty();
		/*
		bool empty = false;
		MutexAutoLock al(_mutex);
		empty = _textures.empty();
		return empty;
		*/
	}

	void Mem2Native::updateTexture(Point &prev, MemoryTexture *src, NativeTexture *dest)
	{
		int SIZE = _size;
		if (!SIZE)
			SIZE = RECT_SIZE;

		if (isCompressedFormat(src->getFormat()))
		{
			dest->init(src->lock(), false);
		}
		else
		{

			Rect textureRect(0, 0, src->getWidth(), src->getHeight());

			if (dest->getHandle() == 0)
				dest->init(textureRect.getWidth(), textureRect.getHeight(), src->getFormat());		


			Rect srcRect(prev.x, prev.y, std::min(SIZE, textureRect.getWidth()), std::min(SIZE, textureRect.getHeight()));
			srcRect.clip(textureRect);


			ImageData srcim = src->lock(&srcRect);


			ImageData destim;
			if (srcRect != textureRect)
			{
				int pitch = srcRect.getWidth() * getBytesPerPixel(dest->getFormat());
				_buffer.resize(srcRect.getHeight() * pitch);
				destim = ImageData(
					srcRect.getWidth(), srcRect.getHeight(),
					pitch, 
					dest->getFormat(), 
					&_buffer[0]
				);

				operations::copy(srcim, destim);
			}		
			else
			{
				destim = srcim;
			}

			dest->updateRegion(srcRect.pos.x, srcRect.pos.y, destim);
			
			prev.x += SIZE;
			if (prev.x >= textureRect.getWidth())
			{
				prev.x = 0;
				prev.y += SIZE;

			}

			if (prev.y >= textureRect.getBottom())
			{
				_buffer.clear();
				prev = Point(0,0);

				src->releaseRef();
				dest->releaseRef();
			}
		}
	}

	void Mem2Native::update()
	{
		//update only one texture per frame
		if (_prev == Point(0,0))
		{
			ThreadMessages::message ev;
			while (_messages.peek(ev, true))
			{
				MemoryTexture *src = (MemoryTexture *)ev.arg1;
				NativeTexture *dest = (NativeTexture *)ev.arg2;
				
				updateTexture(_prev, src, dest);
				if(_prev != Point(0,0))
				{
					_src = src;
					_dest = dest;
					break;
				}
			}
		}
		else
		{
			updateTexture(_prev, _src, _dest);
		}
	}
}