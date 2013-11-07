#pragma once
#include "oxygine_include.h"
#include "Sprite.h"

namespace oxygine
{
	DECLARE_SMART(ProgressBar, spProgressBar);

	class ProgressBar:public Sprite
	{
	public:
		enum direction
		{
			dir_0,//moveable right edge
			dir_90,//moveable top edge
			dir_180,//moveable left edge
			dir_270,//moveable bottom edge
			dir_radial_cw,
			dir_radial_ccw//not implemented!!
		};

		DECLARE_COPYCLONE(ProgressBar);

		ProgressBar();
		~ProgressBar();

		float		getProgress() const;
		direction	getDirection() const;

		/** setProgress
		@param value range [0.0f, 1.0f]
		*/
		void setProgress(float value);
		void setDirection(direction dir);

		string dump(const dumpOptions &) const;
		
		typedef GetSet<float, float, ProgressBar, &ProgressBar::getProgress, &ProgressBar::setProgress> TweenProgress;		

	private:
		void doRender(const RenderState &rs);

		RectF getDestRect() const;
		void _update();

		void animFrameChanged(const AnimationFrame &f);

		float _progress;
		direction _direction;
		AnimationFrame _originalFrame;
	};
}