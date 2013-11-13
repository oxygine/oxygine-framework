#include "Mem2Native.h"
#include "ImageDataOperations.h"

#if __S3E__
#include "s3eDevice.h"
#elif OXYGINE_SDL
#include "SDL_timer.h"
#else
#endif

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
		{
			MutexAutoLock al(_mutex);
			texture t;
			t.src = src.get();
			t.dest = dest.get();		

			_textures.push_back(t);
		}


		sleep(10);

		volatile int size = 0;
		do 
		{			
			{
				MutexAutoLock al(_mutex);
				size = _textures.size();
			}		

			sleep(10);
			
		} while (size > 0);

		//log::messageln("push done");
	}

	void Mem2Native::updateNext()
	{
	}

	bool Mem2Native::isEmpty() const
	{
		bool empty = false;
		MutexAutoLock al(_mutex);
		empty = _textures.empty();
		return empty;
	}

	void Mem2Native::update()
	{
		MutexAutoLock al(_mutex);
		
		if (!_textures.empty())
		{
			int SIZE = _size;
			if (!SIZE)
				SIZE = RECT_SIZE;

			texture &current = _textures.front();
			if (isCompressedFormat(current.src->getFormat()))
			{
				current.dest->init(current.src->lock(), false);
				_textures.pop_front();
			}
			else
			{

				Rect textureRect(0, 0, current.src->getWidth(), current.src->getHeight());

				if (current.dest->getHandle() == 0)
					current.dest->init(textureRect.getWidth(), textureRect.getHeight(), current.src->getFormat());		
			

				Rect srcRect(_prev.x, _prev.y, min(SIZE, textureRect.getWidth()), min(SIZE, textureRect.getHeight()));
				srcRect.clip(textureRect);
								

				ImageData src = current.src->lock(&srcRect);
		

				ImageData dest;
				if (srcRect != textureRect)
				{
					int pitch = srcRect.getWidth() * getBytesPerPixel(current.dest->getFormat());
					_buffer.resize(srcRect.getHeight() * pitch);
					dest = ImageData(
						srcRect.getWidth(), srcRect.getHeight(),
						pitch, 
						current.dest->getFormat(), 
						&_buffer[0]
					);

					operations::copy(src, dest);
				}		
				else
				{
					dest = src;
				}

				current.dest->updateRegion(srcRect.pos.x, srcRect.pos.y, dest);
				_buffer.clear();

				_prev.x += SIZE;
				if (_prev.x >= textureRect.getWidth())
				{
					_prev.x = 0;
					_prev.y += SIZE;

				}

				if (_prev.y >= textureRect.getBottom())
				{
					_textures.pop_front();
					_prev = Point(0,0);
					//log::messageln("_textures.pop_front();");
				}
			}
		}
	}
}