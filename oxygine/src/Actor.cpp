#include "Actor.h"
#include "core/Texture.h"
#include "core/Renderer.h"
#include "res/ResAnim.h"
#include "RootActor.h"
#include "Clock.h"
#include "Tweener.h"
#include "math/AffineTransform.h"
#include <sstream>
#include <typeinfo>
#define _USE_MATH_DEFINES
#include <math.h>
#include "utils/stringUtils.h"
#include "RenderState.h"
//#include ""

namespace oxygine
{
	string div(const string &val, const Color &color)
	{
		char str[255];
		sprintf(str, "<div c='%s'>%s</div>", color2hex(color).c_str(), val.c_str());
		return str;
	}

	Actor::Actor():
			_pos(0, 0),
			_size(0, 0),
            _extendedIsOn(0),
			_zOrder(0),
			_anchor(0, 0),
			_scale(1, 1),
			_rotation(0),
			_flags(flag_visible | flag_inputEnabled | flag_inputChildrenEnabled | flag_childrenRelative),
			_parent(0),
			_alpha(255),
			_pressed(0),
			_overed(0)
	{
		_transform.identity();
		_transformInvert.identity();

		//addEventListener(et_MouseDown, CLOSURE(this, &Actor::_onMouseEvent));
		//addEventListener(et_MouseMove, CLOSURE(this, &Actor::_onMouseEvent));
	}

	Actor::Actor(const Actor &src, cloneOptions opt):EventDispatcher(src)
	{
		_pos = src._pos;
		_extendedIsOn = src._extendedIsOn;
		_size = src._size;
		_zOrder = src._zOrder;
		_anchor = src._anchor;
		_scale = src._scale;
		_rotation = src._rotation;
		_flags = src._flags;
		_parent = 0;
		_alpha = src._alpha;
		_overed = 0;
		_pressed = 0;

		_transform = src._transform;
		_transformInvert = src._transformInvert;
		

		if (!(opt & cloneOptionsDoNotCloneClildren))
		{
			spActor child = src.getFirstChild();
			while(child)
			{
				spActor copy = child->clone(opt);
				addChild(copy);
				child = child->getNextSibling();
			}
		}		

		if (opt & cloneOptionsResetTransform)
		{
			setPosition(0,0);
			setRotation(0);
			setScale(1);
		}
	}

	Actor::~Actor()
	{
		//printf("Actor::~Actor %s\n", getName().c_str());
		removeTweens();
		//removeEventHandlers();
		removeChildren();
		if (RootActor::instance)
			RootActor::instance->removeEventListeners(this);
	}	

	std::string Actor::dump(const dumpOptions &opt) const
	{
		stringstream stream;
		stream << "{" << typeid(*this).name() << "}";
		//stream << this;
		
		if (__name && __name->size())
		{
			stream << " name='" << div(*__name, Color(0xff0000ff)) << "'";			
		}

		stream << " id='" << getObjectID()<< "'";
		stream << "\n";

		if (!getVisible())
			stream << " invisible";

		if (getAlpha() != 255)
			stream << " alpha=" << (int)getAlpha();

		if (getWidth() || getHeight())		
			stream << " size=(" << getWidth() << "," << getHeight() << ")";

		if (getPriority())		
			stream << " priority=" << getPriority();
		
		if (_extendedIsOn)		
			stream << " extendedClickArea=" << (int)_extendedIsOn;

		if (getX() != 0.0f || getY() != 0.0f)
			stream << " pos=(" << getX() << "," << getY() << ")";

		if (getScaleX() != 1.0f || getScaleY() != 1.0f)
			stream << " scale=(" << getScaleX() << "," << getScaleY() << ")";

		if (getAnchor().x || getAnchor().y)
			stream << " anchor=(" << getAnchor().x << "," << getAnchor().y << ")";

		if (getRotation() != 0.0f)
			stream << " rot=" << getRotation() / M_PI * 360.0f << "";

		int tweensCount = 0;
		spTween t = _tweens._first;
		while (t)
		{
			t = t->getNextSibling();
			tweensCount++;
		}

		if (tweensCount)
			stream << " tweens=" << tweensCount << "";

		if (getListenersCount())
			stream << " listeners=" << (int)getListenersCount() << "";


		

		/*
		int handlersCount = 0;
		spEventHandler eh = _eventHandlers._first;
		while (eh)
		{
			eh = eh->getNextSibling();
			handlersCount++;
		}
		if (handlersCount)
			stream << " handlers=" << handlersCount << "";
		*/

		if (getClock())
			stream << " " << getClock()->dump();

		return stream.str();
	}


	RectF Actor::getScreenSpaceDestRect(const Renderer::transform &tr) const
	{
		RectF rect = getDestRect();
		Vector2 tl = rect.pos;
		Vector2 br = rect.pos + rect.size;

		tl = tr.transform(tl);
		br = tr.transform(br);

		Vector2 size = br - tl;

		return RectF(tl, size);
	}

	pointer_index Actor::getPressed() const
	{
		return _pressed;
	}

	pointer_index Actor::getOvered() const
	{
		return _overed;
	}

	void Actor::setPressed(pointer_index v)
	{
		if (_pressed == v)
			return;
		pointer_index old = _pressed;
		_pressed = v;

		//spMouseEvent
		//dispatchEvent()

		if (v)
		{
			RootActor::instance->addEventListener( TouchEvent::TOUCH_UP, CLOSURE(this, &Actor::_onMouseEvent));
			//printf("added\n");
		}
		else
			RootActor::instance->removeEventListener( TouchEvent::TOUCH_UP, CLOSURE(this, &Actor::_onMouseEvent));

		if (!old)
			updateState();
	}

	void Actor::setOvered(pointer_index v)
	{
		if (_overed == v)
			return;

		pointer_index old = _overed;
		_overed = v;
		
		if (v)
		{
			TouchEvent e(TouchEvent::OVER);
			e.index = v;
			dispatchEvent(&e);

			RootActor::instance->addEventListener( TouchEvent::MOVE, CLOSURE(this, &Actor::_onMouseEvent));
		}
		else
		{
			RootActor::instance->removeEventListener( TouchEvent::MOVE, CLOSURE(this, &Actor::_onMouseEvent));
			//if (!_overed)
			{
				TouchEvent e(TouchEvent::OUT);
				e.index = old;
				dispatchEvent(&e);
			}
		}

		if (!old)
			updateState();
	}

	void Actor::_onMouseEvent(Event *event)
	{
		TouchEvent *me = safeCast<TouchEvent*>(event);
		Actor *act = safeCast<Actor*>(event->target.get());

		switch(event->type)
		{
		case  TouchEvent::TOUCH_DOWN:
			if (isDescendant(act))
			{
				if (event->phase == Event::phase_target)
					RootActor::instance->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Actor::_onMouseEvent));					
				setPressed(me->index);
				//_pressed = me->id;
				//setPressed(true);
			}	
			break;
		case  TouchEvent::TOUCH_UP:
			{
				//if (_pressed))
				{
					if (_pressed == me->index)
					{
						setPressed(0);			
						//it is event from ROOT, convert to local space
						Vector2 lp = convert_global2local(this, getRoot(), me->localPosition);
						if (isDescendant(act))
						{
							TouchEvent e(TouchEvent::CLICK, true, lp);
							e.index = me->index;
							dispatchEvent(&e);
						}
						else
						{
							dispatchEvent(event);
						}
					}					
				}
			}
			break;
		case  TouchEvent::MOVE:
			{
				if (isDescendant(act))
				{					
					if (!_overed)
					{
						setOvered(me->index);

						if (event->phase == Event::phase_target)
							RootActor::instance->addEventListener(TouchEvent::MOVE, CLOSURE(this, &Actor::_onMouseEvent));
					}
				}
				else
				{
					if (_overed == me->index)
						setOvered(0);
				}				
			}
			break;
		}

	}

	void Actor::dispatchEvent(Event *event)
	{
		spEventDispatcher prevTarget = event->target;
		if (prevTarget == 0 || event->currentTarget != 0)
			event->target = this;

		
		//event->currentTarget = this;

		EventDispatcher::dispatchEvent(event);


		if (!event->stopsImmediatePropagation && event->bubbles && !event->stopsPropagation)
		{

			if (_parent)
			{
				bool touchEvent = event->type > _et_TouchFirst && event->type < _et_TouchLast;

				if(touchEvent)
				{	
					TouchEvent *me = safeCast<TouchEvent *>(event);
					me->localPosition = local2global(me->localPosition);
				}
				event->phase = Event::phase_bubbling;

				event->currentTarget = 0;
				_parent->dispatchEvent(event);
			}
		}
		if (prevTarget)
			event->target = prevTarget;
	}

	void Actor::handleEvent(Event *event)
	{
		bool touchEvent = event->type > _et_TouchFirst && event->type < _et_TouchLast;
		if (touchEvent)
		{
			if (!(_flags & flag_visible) || getAlpha() == 0)
			{
				return;
			}
		}
		
		Vector2 originalLocalPos(0, 0);

		if(touchEvent)
		{				
			TouchEvent *me = safeCast<TouchEvent *>(event);
			originalLocalPos = me->localPosition;
			me->localPosition = global2local(originalLocalPos);
		}

		event->phase = Event::phase_capturing;
		spActor actor = _children._last;
		while (actor)
		{
			spActor prev = actor->_prev;
			if (!touchEvent || (_flags & flag_inputChildrenEnabled))
				actor->handleEvent(event);
			//if (event->target)
			//	break;
			actor = prev;
		}

		/*
		if (getID() == 46 && event->type == TouchEvent::TOUCH_DOWN)
			int q=0;
			*/
		if (touchEvent)
		{
			TouchEvent *me = safeCast<TouchEvent *>(event);
			if (!event->target)
			{
				if ((_flags & flag_inputEnabled) && isOn(me->localPosition))
				{
					event->phase = Event::phase_target;
					event->target = this;


					if (event->type == TouchEvent::TOUCH_DOWN || event->type == TouchEvent::MOVE)
						_onMouseEvent(event);


					dispatchEvent(event);
				}
			}
			else
			{
				if (event->type == TouchEvent::TOUCH_UP && getParent() == 0)
					dispatchEvent(event);
			}

			me->localPosition = originalLocalPos;			
		}
	}

	void Actor::setAnchor(const Vector2 &anchor)
	{
		_anchor = anchor;
		
		_flags &= ~flag_anchorInPixels;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setAnchor(float ax, float ay)
	{
		setAnchor(Vector2(ax, ay));
	}

	void Actor::setAnchorInPixels(const Vector2 &anchor)
	{		
		_anchor = anchor;
		_flags |= flag_anchorInPixels;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setPosition(const Vector2 &pos)
	{
		if (_pos == pos)
			return;
		_pos = pos;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setPosition(float x, float y)
	{
		setPosition(Vector2(x, y));
	}

	void Actor::setX(float x)
	{
		_pos.x = x;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setY(float y)
	{
		_pos.y = y;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}


	void Actor::setPriority(short zorder)
	{
		_zOrder = zorder;
		if (_parent)
		{
			Actor *parent = _parent;
			addRef();
			parent->removeChild(this);
			parent->addChild(this);
			releaseRef();
		}
	}

	void Actor::setScale(float scale)
	{
		setScale(Vector2(scale, scale));
	}

	void Actor::setScale(const Vector2 &scale)
	{
		if (_scale == scale)
			return;
		_scale = scale;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setScale(float scaleX, float scaleY)
	{
		setScale(Vector2(scaleX, scaleY));
	}

	void Actor::setScaleX(float sx)
	{
		if (_scale.x == sx)
			return;
		_scale.x = sx;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setScaleY(float sy)
	{
		if (_scale.y == sy)
			return;
		_scale.y = sy;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::setRotation(float rotation)
	{
		if (_rotation == rotation)
			return;

		_rotation = rotation;
		_flags |= flag_transformDirty | flag_transformInvertDirty;
	}

	void Actor::sizeChanged(const Vector2 &size)
	{

	}

	void Actor::setSize(const Vector2 &size)
	{
		//INT_MAX
		//OX_ASSERT(size.x < SHORT_MAX);
		//OX_ASSERT(size.y < SHORT_MAX);
		//OX_ASSERT(size.x > -SHORT_MAX);
		//OX_ASSERT(size.y > -SHORT_MAX);
		_size = size;//.cast<PointS>();
		_flags |= flag_transformDirty | flag_transformInvertDirty;
		sizeChanged(size);
	}	

	void Actor::setSize(float w, float h)
	{
		setSize(Vector2(w, h));
	}

	void Actor::setWidth(float w)
	{
		setSize(Vector2(w, _size.y));
	}

	void Actor::setHeight(float h)
	{
		setSize(Vector2(_size.x, h));
	}

	void Actor::setClock(spClock clock)
	{
		_clock = clock;
	}

	const Renderer::transform &Actor::getTransform() const
	{
		updateTransform();
		return _transform;
	}

	const Renderer::transform &Actor::getTransformInvert() const
	{
		if (_flags & flag_transformInvertDirty)
		{
			_flags &= ~flag_transformInvertDirty;
			_transformInvert = getTransform();
			_transformInvert.invert();
		}
		
		return _transformInvert;
	}

	const spClock&	Actor::getClock() const 
	{
		return _clock;
	}

	void Actor::updateTransform() const
	{
		if (!(_flags & flag_transformDirty))
			return;

		bool rot = _rotation != 0.0f;
		bool sc = _scale.x != 1.0f || _scale.y != 1.0f;
		

		float c = 1.0f;
		float s = 0.0f;
		if (_rotation)			
		{
			c = cosf(_rotation);
			s = sinf(_rotation);
		}

		AffineTransform tr(
			c * _scale.x, s * _scale.x,
			-s * _scale.y, c * _scale.y,
			_pos.x, _pos.y);

		if (_flags & flag_childrenRelative)
		{
			Vector2 offset;
			if (_flags & flag_anchorInPixels)
			{
				offset.x = -_anchor.x;
				offset.y = -_anchor.y;
			}
			else
			{
				offset.x = -float(_size.x * _anchor.x);
				offset.y = -float(_size.y * _anchor.y);//todo, what to do? (per pixel quality)
			}
			
			tr.translate(offset);
		}

		_transform = tr;
		_flags &= ~flag_transformDirty;
	}

	bool Actor::isOn(const Vector2 &localPosition)
	{
		RectF r = getDestRect();
		r.expand(Vector2(_extendedIsOn, _extendedIsOn), Vector2(_extendedIsOn, _extendedIsOn));

		if (r.pointIn(localPosition))
		{
			return true;
		}
		return false;
	}

	bool Actor::isDescendant(spActor actor)
	{
		Actor *act = actor.get();
		while (act)
		{
			if (act == this)
				return true;
			act = act->getParent();
		}
		return false;
	}

	Actor* Actor::getDescendant(const string &name, error_policy ep)
	{
		Actor *actor = _getDescendant(name);
		if (!actor)
		{
			handleErrorPolicy(ep, "can't find descendant: %s", name.c_str());
		}
		return actor;
	}

	Actor* Actor::_getDescendant(const string &name)
	{
		if (isName(name.c_str()))
			return this;

		Actor *child = _children._first.get();
		while (child)
		{
			Actor *des = child->_getDescendant(name);
			if (des)
				return des;
			child = child->getNextSibling().get();
		}
		return 0;
	}

	spActor	 Actor::getChild(const string &name, error_policy ep) const 
	{
		spActor actor = _children._first;
		while (actor)
		{
			if (actor->isName(name))
				return actor;
			actor = actor->_next;
		}

		handleErrorPolicy(ep, "can't find child: %s", name.c_str());

		return 0;
	}

	void Actor::insertChildAfter(spActor actor, spActor insertAfter)
	{
		OX_ASSERT(actor);
		if (!actor)
			return;

		if (insertAfter)
		{ 
			OX_ASSERT(insertAfter->getParent() == this);
		}

		actor->detach();
		/*
		OX_ASSERT(actor->getParent() == 0);
		if (actor->getParent())
			return;
			*/

		if (insertAfter)
			_children.insert_after(actor, insertAfter);
		else
			_children.append(actor);
		setParent(actor.get(), this);
	}

	void Actor::insertChildBefore(spActor actor, spActor insertBefore)
	{
		OX_ASSERT(actor);
		if (!actor)
			return;

		if (insertBefore)
		{
			OX_ASSERT(insertBefore->getParent() == this);
		}

		//actor->removeFromParent();
		
		
		OX_ASSERT(actor->getParent() == 0);
		if (actor->getParent())
			return;
			

		if (insertBefore)
			_children.insert_before(actor, insertBefore);
		else
			_children.prepend(actor);
		setParent(actor.get(), this);
	}

	void Actor::attachTo(spActor parent)
	{
		attachTo(parent.get());
	}

	void Actor::attachTo(Actor *parent)
	{
		OX_ASSERT(parent);
		if (!parent)
			return;
		parent->addChild(this);	
	}

	void Actor::addChild(Actor *actor)
	{
		OX_ASSERT(actor);
		if (!actor)
			return;
		
		OX_ASSERT(actor != this);

		actor->detach();
		//assert(actor->_parent == 0 && "child should be removed from previous parent");

		int z = actor->getPriority();

		spActor sibling = _children._last;

		//try to insert at the end of list first
		if (sibling && sibling->getPriority() > z)
		{		
			sibling = sibling->getPrevSibling();
			while (sibling)
			{
				if (sibling->getPriority() <= z)			
					break;
				sibling = sibling->getPrevSibling();
			}
		}


		if (sibling)
			insertChildAfter(actor, sibling);
		else
			insertChildBefore(actor, 0);
	}

	void Actor::addChild(spActor actor)
	{
		addChild(actor.get());
	}
	
	void Actor::removeChild(spActor actor)
	{
		OX_ASSERT(actor);		
		if (actor)
		{	
			OX_ASSERT(actor->_parent == this);
			if (actor->_parent == this)
			{
				setParent(actor.get(), 0);
				_children.remove(actor);
			}			
		}
	}



	void Actor::removeChildren()
	{
		spActor child = getFirstChild();
		while (child)
		{
			spActor copy = child;
			child = child->getNextSibling();
			removeChild(copy);			
		}
	}


	Actor *Actor::detach()
	{
		Actor *parent = getParent();
		if (parent)
			parent->removeChild(this);
		return parent;
	}

	void Actor::internalUpdate(const UpdateState &us)
	{
		spTween tween = _tweens._first;
		while(tween)
		{
			spTween tweenNext = tween->getNextSibling();

			bool done = false;
			if (tween->getParentList())
				done = tween->update(*this, us);
			if (done && tween->getParentList())
				_tweens.remove(tween);
			tween = tweenNext;
		}

		if (_cbDoUpdate)
			_cbDoUpdate(us);
		doUpdate(us);

		spActor actor = _children._first;
		while (actor)
		{
			spActor next = actor->_next;
			if (actor->getParent())
				actor->update(us);
			if (!next)
			{
				//OX_ASSERT(actor == _children._last);
			}
			actor = next;
		}
	}

	void Actor::update(const UpdateState &parentUS)
	{
		UpdateState us = parentUS;
		if (_clock)
		{			
			us.iteration = 0;
			_clock->update();

			timeMS dt = _clock->doTick();
			while(dt > 0)
			{
				us.dt = dt;
				us.time = _clock->getTime();
				
				internalUpdate(us);

				dt = _clock->doTick();
				us.iteration += 1;
			}			
		}
		else
		{
			internalUpdate(us);
		}
		
	}

	void Actor::doUpdate(const UpdateState &us)
	{

	}

	Vector2 Actor::global2local(const Vector2 &global) const
	{
		const AffineTransform &t = getTransformInvert();
		return t.transform(global);
	}

	Vector2 Actor::local2global(const Vector2 &local) const
	{
		const AffineTransform &t = getTransform();
		return t.transform(local);		
	}

	bool Actor::prepareRender(RenderState &rs, const RenderState &parentRS)
	{
		if (!(_flags & flag_visible))
			return false;

		unsigned char alpha = (parentRS.alpha * _alpha) / 255;
		if (!alpha)
			return false;

		rs = parentRS;		
		rs.alpha = alpha;		

		Renderer::transform tr = getTransform() * parentRS.transform;
		rs.transform = tr;

		if (_flags & flag_cull)
		{
			RectF ss_rect = getScreenSpaceDestRect(tr);
			RectF intersection = ss_rect;
			intersection.clip(rs.clip);
			if (intersection.isEmpty())
				return false;

		}

		
		//tr.x = int(tr.x);
		//tr.y = int(tr.y);
		
		rs.renderer->setTransform(tr);

		return true;
	}

	void Actor::completeRender(const RenderState &rs)
	{

	}

	bool Actor::internalRender(RenderState &rs, const RenderState &parentRS)
	{
		if (!prepareRender(rs, parentRS))
			return false;

		if (_cbDoRender)
			_cbDoRender(rs);
		doRender(rs);
		completeRender(rs);
		return true;
	}

	void Actor::render(const RenderState &parentRS)
	{
		RenderState rs;		
		if (!internalRender(rs, parentRS))
			return;

		Actor *actor = _children._first.get();
		while (actor)
		{
			if (actor->getParent())//todo remove???
				actor->render(rs);
			actor = actor->_next.get();
		}
	}

	RectF Actor::calcDestRectF(const RectF &destRect_, const Vector2 &size) const
	{
		RectF destRect = destRect_;
		if (!(_flags & flag_childrenRelative))
		{
			Vector2 a;

			if ((_flags & flag_anchorInPixels))
				a = Vector2(_anchor.x, _anchor.y);
			else
				a = Vector2(_anchor.x * size.x, _anchor.y * size.y);

			destRect.pos -= a;
		}
		return destRect;
	}

	RectF Actor::getDestRect() const
	{
		return calcDestRectF(RectF(Vector2(0, 0), getSize()), getSize());
	}

	spTween Actor::_addTween(spTween tween, bool rel)
	{
		OX_ASSERT(tween);
		if (!tween)
			return 0;

		tween->setRelative(rel);

		bool done = false;
		if (tween->getDelay() == 0)
			done = tween->start(*this);

		if (!done)
			_tweens.append(tween);

		return tween;
	}

	spTween Actor::addTween(spTween tween)
	{
		return _addTween(tween, false);
	}	

	spTween Actor::addTweenRelative(spTween tween)
	{
		return _addTween(tween, true);
	}

	spTween Actor::getTween(const string &name, error_policy ep)
	{
		spTween tween = _tweens._first;
		while (tween)
		{
			if (tween->isName(name))
				return tween;
			tween = tween->getNextSibling();
		}

		handleErrorPolicy(ep, "can't find tween: %s", name.c_str());
		return 0;
	}

	void Actor::removeTween(spTween v)
	{
		OX_ASSERT(v);
		if (!v)
			return;
		
		if (v->getParentList() == &_tweens)
		{
			v->setClient(0);
			_tweens.remove(v);
		}
	}

	void Actor::removeTweens(bool callComplete)
	{
		spTween t = _tweens._first;
		while (t)
		{			
			spTween c = t;

			if (callComplete)
				c->complete();

			t = t->getNextSibling();
			removeTween(c);
		}
	}

	Vector2 convert_global2local_(Actor *child, Actor *parent, Vector2 pos)
	{
		if (child->getParent() && child->getParent() != parent)
			pos = convert_global2local(child->getParent(), parent, pos);
		/*
		Actor *p = child->getParent();
		if (p && child != parent)
			pos = convert_global2local(p, parent, pos);
			*/
		pos = child->global2local(pos);
		return pos;
	}

	Vector2 convert_global2local(spActor child, spActor parent, const Vector2 &pos)
	{
		return convert_global2local_(child.get(), parent.get(), pos);
	}

	Vector2 convert_local2global_(Actor* child, Actor* parent, Vector2 pos)
	{
		while(child && child != parent)
		{
			pos = child->local2global(pos);
			child = child->getParent();
		}

		return pos;
	}

	Vector2 convert_local2global(spActor child, spActor parent, const Vector2 &pos)
	{
		return convert_local2global_(child.get(), parent.get(), pos);
	}

	Vector2 convert_local2root(spActor actor, const Vector2 &pos, spActor root)
	{
		if (!root)
			root = getRoot();
		return convert_local2global(actor, root, pos);
	}

	Vector2 convert_root2local(spActor actor, const Vector2 &pos, spActor root)
	{
		if (!root)
			root = getRoot();
		return convert_global2local(actor, root, pos);
	}

	Renderer::transform getGlobalTransform(spActor child)
	{
		Renderer::transform t;
		t.identity();
		while (child)
		{
			t = t * child->getTransform();
			child = child->getParent();
		}

		return t;
	}

	void changeParentAndSavePosition(spActor mutualParent, spActor actor, spActor newParent)
	{
		Vector2 pos = actor->getPosition();
		spActor act = actor->getParent();
		while(act && act != mutualParent)
		{
			pos = act->local2global(pos);
			act = act->getParent();
		}

		if (newParent != mutualParent)
			pos = convert_global2local(newParent, mutualParent, pos);
		actor->setPosition(pos);
		actor->attachTo(newParent);
	}
}