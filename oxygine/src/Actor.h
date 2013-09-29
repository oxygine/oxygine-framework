#pragma once
#include "oxygine_include.h"
#include "core/oxygine.h"
#include "EventDispatcher.h"
#include "core/Object.h"
#include "math/Color.h"
#include "Clock.h"
#include "math/Rect.h"
#include "Tweener.h"

#include <vector>
#include <list>
#include "math/AffineTransform.h"
#include "core/Renderer.h"
#include "utils/intrusive_list.h"
#include "Tweener.h"
#include "Event.h"

namespace oxygine
{
	class Renderer;
	class Color;
	class Event;

	typedef unsigned int dumpOptions;

	class RenderState;
	class UpdateState;

			
	DECLARE_SMART(Texture, spTexture);
	DECLARE_SMART(Actor, spActor);
	DECLARE_SMART(Clock, spClock);

	typedef std::vector<spActor> actors;

	
	template<class T>
	T *create(T *item, const Vector2 *pos, const char *name)
	{
		if (pos)
			item->setPosition(pos);
		if (name)
			item->setName(name);
		return item;
	}

	typedef Closure<void (const UpdateState &us)> UpdateCallback;
	typedef Closure<void (const RenderState &rs)> RenderCallback;

	const int cloneOptionsDoNotCloneClildren = 0x01;
	const int cloneOptionsResetTransform = 0x02;
	typedef int cloneOptions;
	typedef int copyOptions;//deprecated typedef


#define DECLARE_COPYCLONE(type) type(const type &src, cloneOptions);\
	virtual type* clone(cloneOptions opt=0) const {return new type(*this, opt);}


	

	class Actor : public EventDispatcher, public intrusive_list_item<spActor>
	{
		typedef intrusive_list_item<spActor> intr_list;
	public:
		DECLARE_COPYCLONE(Actor);

		Actor();
		virtual ~Actor();

		/**returns first child*/
		spActor				getFirstChild() const {return _children._first;}
		/**returns last child*/
		spActor				getLastChild() const {return _children._last;}
		/**returns next sibling*/
		spActor				getNextSibling() {return intr_list::getNextSibling();}
		/**returns previous sibling*/
		spActor				getPrevSibling() {return intr_list::getPrevSibling();}

		
		/**search child by name recursively, could return self*/
		Actor*				getDescendant(const string &name, error_policy ep = ep_show_error);
		/**search child by name recursively and cast it to other class*/
		template<class T>
		T*					getDescendantT(const string &name, error_policy ep = ep_show_error) {return safeCast<T*> (getDescendant(name, ep));}
		/**search child by name*/
		spActor				getChild(const string &name, error_policy ep = ep_show_error) const;
		/**search child by name and cast it to other class*/
		template<class T>
		T*					getChildT(const string &name, error_policy ep = ep_show_error) const {return safeCast<T*> (getChild(name, ep).get());}

		/**search tween by name*/
		spTween				getTween(const string &name, error_policy ep = ep_show_error);
		/**returns first tween in actor*/
		spTween				getFirstTween() const {return _tweens._first;}
		/**returns last tween in actor*/
		spTween				getLastTween() const {return _tweens._last;}

		const Vector2&		getAnchor() const {return _anchor;}
		bool				getIsAnchorInPixels() {return (_flags & flag_anchorInPixels) != 0;}
		const Vector2&		getPosition() const {return _pos;}
		float				getX() const {return _pos.x;}
		float				getY() const {return _pos.y;}		
		const Vector2&		getScale() const {return _scale;}
		const float			getScaleX() const {return _scale.x;}
		const float			getScaleY() const {return _scale.y;}
		/**Returns rotation angle in radians*/
		float				getRotation() const {return _rotation;}		
		int					getPriority() const {return _zOrder;}				
		bool				getVisible() const {return (_flags & flag_visible) != 0;}
		Actor*				getParent() const {return _parent;}
		Vector2				getSize() const {return _size;}
		float				getWidth() const {return getSize().x;}
		float				getHeight() const {return getSize().y;}		
		unsigned char		getAlpha() const {return _alpha;}
		const spClock&		getClock() const;
		virtual RectF		getDestRect() const;
		bool				getInputEnabled() const {return (_flags & flag_inputEnabled) != 0;}
		bool				getChildrenRelative() const {return (_flags & flag_childrenRelative) != 0;;}
		UpdateCallback		getCallbackDoUpdate() const {return _cbDoUpdate;}
		RenderCallback		getCallbackDoRender() const {return _cbDoRender;}

		const Renderer::transform&		getTransform() const;
		const Renderer::transform&		getTransformInvert() const;
		

		/**Sets Anchor. Anchor is "center" of rotation/scale/position*/
		void setAnchor(const Vector2 &anchor);
		void setAnchor(float ax, float ay);
		void setAnchorInPixels(const Vector2 &anchor);
		void setPosition(const Vector2 &pos);
		void setPosition(float x, float y);
		void setX(float x);
		void setY(float y);

		/** set z order draw priority, from back (low value) to front (high value). Max value is 32000, Min value -32000*/
		void setPriority(short zorder);
		void setScale(float scale);
		void setScale(const Vector2 &scale);
		void setScale(float scaleX, float scaleY);
		void setScaleX(float sx);
		void setScaleY(float sy);
		/**Sets rotation angle in radians*/
		void setRotation(float angle);
		/**This option is connected with Anchor. By default value is True*/
		void setChildrenRelative(bool r){_flags &= ~flag_childrenRelative; if (r) _flags |= flag_childrenRelative;}
		
		/**Sets Size of Actor. Size doesn't scale contents of Actor. Size only affects event handling and rendering if you change Anchor*/
		void setSize(const Vector2 &);
		void setSize(float w, float h);
		void setWidth(float w);
		void setHeight(float h);
		/**Extends actor's clickable area from each side. Affects only to Actor::isOn. Max value is 127, Min Value is -128*/
		void setExtendedClickArea(char add) {_extendedIsOn = add;}

		void setClock(spClock clock);

		/**Show/Hide actor and children. Invisible Actor doesn't receive events.*/
		void setVisible(bool vis) {_flags &= ~flag_visible; if (vis) _flags |= flag_visible;}
		/**Enable/Disable culling this actor outside of clip area (use it with ClipRectActor)*/
		void setCull(bool enable) {_flags &= ~flag_cull; if (enable) _flags |= flag_cull;}
		/**Sets transparency. if alpha is 0 actor and children are completely invisible, don't rendering and don't receive events.*/
		void setAlpha(unsigned char alpha){_alpha = alpha;}
		/**Enables/Disables input events(touch, mouse) for Actor.*/
		void setInputEnabled(bool enabled) {_flags &= ~flag_inputEnabled; if (enabled) _flags |= flag_inputEnabled;}
		/**Enables/Disables input events(touch, mouse) for children of Actor.*/
		void setInputChildrenEnabled(bool enabled) {_flags &= ~flag_inputChildrenEnabled; if (enabled) _flags |= flag_inputChildrenEnabled;}

		/**Sets callback which would be called each Actor::update cycle before doUpdate. Use it if you don't want inherit from Actor and overload Actor::doUpdate.*/
		void setCallbackDoUpdate(UpdateCallback cb){_cbDoUpdate = cb;}
		/**Sets callback which would be called each Actor::render cycle before doRender. Use it if you don't want inherit from Actor and overload Actor::doRender.*/
		void setCallbackDoRender(RenderCallback cb){_cbDoRender = cb;}

		virtual bool isOn(const Vector2 &localPosition);
		/**Returns true if actor is child or located deeper in current subtree*/
		bool isDescendant(spActor actor);
		
		
		void insertChildBefore(spActor actor, spActor insertBefore);
		void insertChildAfter(spActor actor, spActor insertAfter);
		void addChild(spActor actor);
		void addChild(Actor *actor);//avoid conversion to spActor
		void attachTo(spActor parent);
		void attachTo(Actor *parent);//avoid conversion to spActor
		void removeChild(spActor actor);
		void removeChildren();
		
		
		/**detaches actor from parent and returns parent. return NULL If actor doesn't have parent*/
		Actor* detach();

		/**Dispatches an event into the event flow. The event target is the EventDispatcher object upon which the dispatchEvent() method is called.*/
		void dispatchEvent(Event *event);


		pointer_index getPressed() const;
		pointer_index getOvered() const;
		void setPressed(pointer_index v);
		void setOvered(pointer_index v);

		virtual void updateState(){}
		
		spTween addTween(spTween);		
		spTween addTweenRelative(spTween);		

		template<class GS>
		spTween addTween(const GS &gs, timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, Tween::EASE ease = Tween::ease_linear)
		{return addTween(createTween(gs, duration, loops, twoSides, delay, ease));}

		void removeTween(spTween);
		/**remove all tweens and call completes them if callComplete == true*/
		void removeTweens(bool callComplete = false);
		
		/**Updates this actor, children and all tweens.*/
		virtual void update(const UpdateState &us);
		/**Renders this actor and children.*/
		virtual void render(const RenderState &rs);
		virtual void handleEvent(Event *event);		
		virtual void doRender(const RenderState &rs){}

		/**Returns detailed actor information. Used for debug purposes. */
		virtual std::string dump(const dumpOptions &opt) const;


		//converts global position (position in parent space) to local space
		Vector2 global2local(const Vector2 &pos) const;
		//converts local position to parent space
		Vector2 local2global(const Vector2 &pos) const;

				
		typedef GetSet2Args<float, Vector2, const Vector2 &, Actor, &Actor::getPosition, &Actor::setPosition>	TweenPosition;
		typedef GetSet<float, float, Actor, &Actor::getX, &Actor::setX>											TweenX;
		typedef GetSet<float, float, Actor, &Actor::getY, &Actor::setY>											TweenY;
		typedef GetSet<float, float, Actor, &Actor::getWidth, &Actor::setWidth>									TweenWidth;
		typedef GetSet<float, float, Actor, &Actor::getHeight, &Actor::setHeight>								TweenHeight;
		typedef GetSet<float, float, Actor, &Actor::getRotation, &Actor::setRotation>							TweenRotation;
		typedef GetSet2Args1Arg<float, Vector2, const Vector2 &, Actor, &Actor::getScale, &Actor::setScale>		TweenScale;
		typedef GetSet<unsigned char, unsigned char, Actor, &Actor::getAlpha, &Actor::setAlpha>					TweenAlpha;
		

	protected:
		Actor*	_getDescendant(const string &name);
		spTween _addTween(spTween tween, bool rel);

		
		virtual void doUpdate(const UpdateState &us);
		UpdateCallback _cbDoUpdate;
		RenderCallback _cbDoRender;

		virtual void sizeChanged(const Vector2 &size);

		bool prepareRender(RenderState &rs, const RenderState &parentRS);
		void completeRender(const RenderState &rs);
		bool internalRender(RenderState &rs, const RenderState &parentRS);

		void updateTransform() const;
		void internalUpdate(const UpdateState &us);

		RectF getScreenSpaceDestRect(const Renderer::transform &) const; 

		//static void internalRemoveChild(spActor parent, spActor actor, bool fast = false);


		pointer_index _pressed;
		pointer_index _overed;


	private:
		Vector2 _pos;		
		Vector2 _anchor;		
		Vector2 _scale;
		Vector2 _size;
		float	_rotation;		
		short	_zOrder;

		enum flags
		{
			flag_anchorInPixels = 0x01,
			flag_visible = 0x02,
			flag_inputEnabled = 0x04,
			flag_childrenRelative = 0x08,
			flag_transformDirty = 0x10,
			flag_transformInvertDirty = 0x20,
			flag_inputChildrenEnabled = 0x40,
			flag_cull = 0x80,
		};

		mutable unsigned char _flags;
		mutable Renderer::transform	_transform;
		mutable Renderer::transform	_transformInvert;

	protected:		
		unsigned char	_alpha;
		char	_extendedIsOn;

		spClock _clock;	

		Actor *_parent;

		typedef intrusive_list<spTween> tweens;
		tweens _tweens;


		static void setParent(Actor *actor, Actor *parent){actor->_parent = parent;}


		void _onMouseEvent(Event *ev);
		
		typedef intrusive_list<spActor> children;
		children _children;

		static children &getChildren(spActor &actor){return actor->_children;}

		RectF calcDestRectF(const RectF &destRect, const Vector2 &size) const;
	};

	Vector2 convert_global2local(spActor child, spActor parent, const Vector2 &pos);//deprecated, use convert_root2local
	Vector2 convert_local2global(spActor child, spActor parent, const Vector2 &pos);//deprecated, use convert_local2root

	Vector2 convert_local2root(spActor child, const Vector2 &pos, spActor root = 0);
	Vector2 convert_root2local(spActor child, const Vector2 &pos, spActor root = 0);

	Renderer::transform getGlobalTransform(spActor child);

	void	changeParentAndSavePosition(spActor mutualParent, spActor actor, spActor newParent);
	

	/** A TweenDummy class
	 *  doing nothing, could be used for calling your callback after timeout
	 */
	class TweenDummy
	{
	public:
		typedef Actor type;

		void init(Actor &, bool rel){}
		void update(Actor &, float p, const UpdateState &us){}
	};
}