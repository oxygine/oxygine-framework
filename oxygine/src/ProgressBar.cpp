#include "ProgressBar.h"
#include <sstream>
namespace oxygine
{
	ProgressBar::ProgressBar():_progress(1.0f), _direction(dir_0)
	{

	}

	ProgressBar::~ProgressBar()
	{

	}
	
	ProgressBar::ProgressBar(const ProgressBar &src, cloneOptions opt):Sprite(src, opt)
	{
		_progress = src._progress;
		_direction = src._direction;
		_originalFrame = src._originalFrame;
	}

	void ProgressBar::animFrameChanged(const AnimationFrame &f)
	{
		_originalFrame = f;
		_update();
	}

	void ProgressBar::_update()
	{
		if (!_frame.getDiffuse().base)
			return;

		RectF newSrc = _originalFrame.getSrcRect();
		RectF newDest = _originalFrame.getDestRect();

		float inv_progress = 1.0f - _progress;
		switch(_direction)
		{
		case dir_180:
			newSrc.pos.x += newSrc.size.x * inv_progress;
			newDest.pos.x += newDest.size.x * inv_progress;
			//break; do not break
		case dir_0:
			newSrc.size.x = newSrc.size.x * _progress;
			newDest.size.x = newDest.size.x * _progress;
			break;
		case dir_90:
			newSrc.pos.y += newSrc.size.y * inv_progress;
			newDest.pos.y += newDest.size.y * inv_progress;
			//break; do not break
		case dir_270:
			newSrc.size.y = newSrc.size.y * _progress;
			newDest.size.y = newDest.size.y * _progress;
			break;
		}			

		Vector2 newSize = _originalFrame.getFrameSize() * _progress;
		_frame.init(0, _frame.getDiffuse(), newSrc, newDest, newSize);
	}

	string ProgressBar::dump(const dumpOptions &options) const
	{
		stringstream stream;
		stream << "{ProgressBar}\n";
	

		const char *dir = "dir_0";
		switch(_direction)
		{
		case dir_90:
			dir = "dir_90";
			break;
		case dir_180:
			dir = "dir_180";
			break;
		case dir_270:
			dir = "dir_270";
			break;
		}
		stream << " direction=" << dir << ""; 

		stream << "\n" << Sprite::dump(options);

		return stream.str();
	}

	RectF ProgressBar::getDestRect() const
	{
		return calcDestRectF(_frame.getDestRect(), _frame.getFrameSize());
	}

	void ProgressBar::setProgress(float f)
	{
		_progress = scalar::clamp(f, 0.0f, 1.0f);
		_update();
	}

	void ProgressBar::setDirection(direction dir)
	{
		_direction = dir;
		_update();
	}

	float ProgressBar::getProgress() const
	{
		return _progress;
	}

	ProgressBar::direction ProgressBar::getDirection() const
	{
		return _direction;
	}
}