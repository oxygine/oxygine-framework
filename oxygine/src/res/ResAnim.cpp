#include "ResAnim.h"
#include "MemoryTexture.h"
#include "core/NativeTexture.h"
#include "core/VideoDriver.h"

namespace oxygine
{
	static AnimationFrame emptyFrame;
	ResAnim::ResAnim(Resource *atlas):_columns(1), _atlas(atlas), _scaleFactor(1.0f)
	{
	}

	ResAnim::~ResAnim()
	{
	}

    void ResAnim::init(spNativeTexture texture, const Point &originalSize, int columns, int rows, float scaleFactor)
    {
		_scaleFactor = scaleFactor;
		if (!texture)
			return;

        int frame_width = originalSize.x / columns;
        int frame_height = originalSize.y / rows;

        animationFrames frames;
        int frames_count = rows * columns;
        frames.reserve(frames_count);

        Vector2 frameSize((float)frame_width, (float)frame_height);
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                Rect src;
                src.pos = Point(x * frame_width, y * frame_height);
                src.size = Point(frame_width, frame_height);

                float iw = 1.0f / texture->getWidth();
                float ih = 1.0f / texture->getHeight();
                RectF srcRect(src.pos.x * iw, src.pos.y * ih, src.size.x * iw, src.size.y * ih);

                RectF destRect(Vector2(0, 0), frameSize * scaleFactor);
                AnimationFrame frame;
                Diffuse df;
                df.base = texture;
                frame.init(this, df, srcRect, destRect, destRect.size);
                frames.push_back(frame);
            }
        }

        init(frames, columns, scaleFactor);
    }

	void ResAnim::init(MemoryTexture *original, int columns, int rows, float scaleFactor)
    {
		_scaleFactor = scaleFactor;
		if (!original)
			return;

		spNativeTexture texture = IVideoDriver::instance->createTexture();
		int w2 = nextPOT(original->getWidth());
		int h2 = nextPOT(original->getHeight());
		if (w2 != original->getWidth() || h2 != original->getHeight())
		{
			texture->init(w2, h2, original->getFormat());
			texture->updateRegion(0, 0, original->lock());
		}
		else
		{
			texture->init(original->lock(), false);
		}
		
        texture->apply();

		init(texture, original->getSize(), columns, rows, scaleFactor);
	}

	void ResAnim::init(animationFrames &frames, int columns, float scaleFactor)
	{
		_columns = columns;
		_frames.swap(frames);
		_scaleFactor = scaleFactor;
	}
	
	void ResAnim::_load(LoadResourcesContext *c)
	{
		if (!_atlas)
			return;
		_atlas->load(c);
	}

	void ResAnim::_unload()
	{
		//OX_ASSERT(!"can't unload resanim");
	}

	/*
	void ResAnim::addFrame(const AnimationFrame &frame)
	{
		_frames.push_back(frame);
	}
	*/

	ResAnim::operator const AnimationFrame &() 
	{
		return getFrame(0,0);
	}

	
	const AnimationFrame &ResAnim::getFrame(int col, int row) const
	{
		int i = row * _columns + col;
		return getFrame(i);
	}

	const AnimationFrame&	ResAnim::getFrame(int index) const
	{
		OX_ASSERT(index < (int)_frames.size());
		if (index < (int)_frames.size())
			return _frames[index];
		return emptyFrame;
	}

	void ResAnim::setFrame(int col, int row, const AnimationFrame &frame)
	{		
		int i = row * _columns + col;
		OX_ASSERT(i < (int)_frames.size());
		if (i < (int)_frames.size())
			_frames[i] = frame;
	}
}
