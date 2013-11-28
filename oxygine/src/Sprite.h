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
		
	

	class Sprite : public VStyleActor
	{
	public:
		DECLARE_COPYCLONE(Sprite);

		Sprite();
		~Sprite();
		
		const AnimationFrame&	getAnimFrame() const {return _frame;}
		virtual RectF			getDestRect() const;
		const Diffuse&			getDiffuse() const {return _frame.getDiffuse();}
		bool					getManageResAnim() const {return (_flags & flag_manageResAnim) != 0;}
		const RectF&			getSrcRect() const {return _frame.getSrcRect();}
		const ResAnim*			getResAnim() const{return _frame.getResAnim();}

		/**load/unload atlas automatically or not*/
		void					setManageResAnim(bool manage);
		/**Changes sprite image*/
		void					setAnimFrame(const AnimationFrame &f);
		/**Takes AnimationFrame from ResAnim and changes it. Shows assert is resanim is null. Using this method is more safe than 'setAnimFrame(const AnimationFrame &f)'*/
		void					setAnimFrame(const ResAnim *resanim, int col = 0, int row = 0);		
		virtual void			setResAnim(const ResAnim *resanim);
		
		
		std::string dump(const dumpOptions &) const;
	protected:
		enum 
		{
			flag_manageResAnim = flag_last << 1
		};
		virtual void changeAnimFrame(const AnimationFrame &f);
		virtual void animFrameChanged(const AnimationFrame &f);
		void doRender(const RenderState &rs);

		AnimationFrame _frame;
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

		void init(Sprite &actor);

		const ResAnim*	getResAnim() const {return _resAnim;}
		int				getRow() const {return _row;}
		int				getStartColumn() const {return _colStart;}
		int				getEndColumn() const {return _colEnd;}

		
		void setResAnim(const ResAnim *resAnim);
		/**play animation in interval [start, end)*/
		void setColumns(int start, int end);
		
		void update(Sprite &actor, float p, const UpdateState &us);

	private:
		const ResAnim *_resAnim;
		int _row;
		int _colStart;
		int _colEnd;
	};
}