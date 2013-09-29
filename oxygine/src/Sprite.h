#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "core/Texture.h"
#include "math/Rect.h"
#include "AnimationFrame.h"
#include "res/ResAnim.h"
#include "VisualStyle.h"

namespace oxygine
{
	class ResAnim;

	DECLARE_SMART(Sprite, spSprite);
		
	class Sprite : public Actor
	{
	public:
		DECLARE_COPYCLONE(Sprite);

		Sprite();
		~Sprite();
		
		//spTexture				getTexture() const {return _frame.getBaseTexture();}
		const Diffuse&			getDiffuse() const {return _frame.getDiffuse();}
		const RectF&			getSrcRect() const {return _frame.getSrcRect();}
		virtual RectF			getDestRect() const;
		const AnimationFrame&	getAnimFrame() const {return _frame;}
		const ResAnim*			getResAnim() const{return _frame.getResAnim();}
		const Color&			getColor() const {return _vstyle.getColor();}
		blend_mode				getBlendMode() const {return _vstyle.getBlendMode();}
		bool					getManageResAnim() const {return _manageResAnim;}

		/**load/unload atlas automatically or not*/
		void					setManageResAnim(bool manage);
		void					setColor(const Color &color){_vstyle.setColor(color);}
		void					setBlendMode(blend_mode mode){_vstyle.setBlendMode(mode);}
		/**Changes sprite image*/
		void					setAnimFrame(const AnimationFrame &f);
		/**Takes AnimationFrame from ResAnim and changes it. Shows assert is resanim is null. Using this method is more safe than 'setAnimFrame(const AnimationFrame &f)'*/
		void					setAnimFrame(const ResAnim *resanim, int col = 0, int row = 0);		
		virtual void			setResAnim(const ResAnim *resanim);
		
		
		std::string dump(const dumpOptions &) const;
		
		typedef GetSet<Color, const Color &, Sprite, &Sprite::getColor, &Sprite::setColor>	TweenColor;

	protected:
		virtual void changeAnimFrame(const AnimationFrame &f);
		virtual void animFrameChanged(const AnimationFrame &f);
		void doRender(const RenderState &rs);

		AnimationFrame _frame;
		VisualStyle _vstyle;
		bool _manageResAnim;
	};



	/** A TweenAnim class
	*   use for playing per frame animation
	\code
	spSprite sprite = new Sprite();
	sprite->addTween(TweenAnim(res.getResAnim("anim")), 500, -1);
	\endcode
	*/
	class TweenAnim
	{
	public:
		typedef Sprite type;

		TweenAnim(const ResAnim *resAnim, int row = 0);

		void init(Sprite &actor, bool rel);

		const ResAnim*	getResAnim() const {return _resAnim;}
		int				getRow() const {return _row;}
		int				getStartColumn() const {return _colStart;}
		int				getEndColumn() const {return _colEnd;}

		
		void setResAnim(const ResAnim *resAnim);
		/**play animation for interval [start, end)*/
		void setColumns(int start, int end);
		
		void update(Sprite &actor, float p, const UpdateState &us);

	private:
		const ResAnim *_resAnim;
		int _row;
		int _colStart;
		int _colEnd;
	};
}