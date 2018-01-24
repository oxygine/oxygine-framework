#pragma once
#include "../oxygine-include.h"
#include "../EventDispatcher.h"
#include "../Serializable.h"
#include "../TouchEvent.h"
#include "../core/Object.h"
#include "../core/Renderer.h"
#include "../math/AffineTransform.h"
#include "../math/Rect.h"
#include "../oxygine-forwards.h"
#include "../tween/Tween.h"
#include "../utils/intrusive_list.h"

namespace oxygine
{
    class TweenOptions
    {
    public:
        explicit TweenOptions(timeMS duration = 500) : _duration(duration), _delay(0), _ease(Tween::ease_linear), _globalEase(Tween::ease_linear), _loops(1), _twoSides(false), _detach(false) {}
        TweenOptions& duration(timeMS duration) { _duration = duration; return *this; }
        TweenOptions& delay(timeMS delay) { _delay = delay; return *this; }
        TweenOptions& loops(int loops) { _loops = loops; return *this; }
        TweenOptions& twoSides(bool enabled = true) { _twoSides = enabled; return *this; }
        TweenOptions& ease(Tween::EASE ease) { _ease = ease; return *this; }
        TweenOptions& detach(bool detach_ = true) { _detach = detach_; return *this; }
        TweenOptions& globalEase(Tween::EASE ease) { _globalEase = ease; return *this; }
        TweenOptions& doneCallback(const EventCallback& cb) { _callback = cb; return *this; }


        EventCallback   _callback;
        timeMS          _duration;
        timeMS          _delay;
        Tween::EASE     _ease;
        Tween::EASE     _globalEase;
        int             _loops;
        bool            _twoSides;
        bool            _detach;
    };

    DECLARE_SMART(Actor, spActor);

    class Actor: public EventDispatcher, public intrusive_list_item<spActor>, public Serializable
    {
        typedef intrusive_list_item<spActor> intr_list;

    public:
        Actor(const Actor& src, cloneOptions opt = 0);
        virtual Actor* clone(cloneOptions opt = 0) const;
        void copyFrom(const Actor& src, cloneOptions opt = 0);

        Actor();
        virtual ~Actor();

        /**returns first child*/
        spActor             getFirstChild() const {return _children._first;}
        /**returns last child*/
        spActor             getLastChild() const {return _children._last;}
        /**returns next sibling*/
        spActor             getNextSibling()const {return intr_list::_next;}
        /**returns previous sibling*/
        spActor             getPrevSibling()const {return intr_list::_prev;}


        /**search child by name recursively, could return self*/
        Actor*              getDescendant(const std::string& name, error_policy ep = ep_show_error);
        /**search child by name recursively and cast it to other class*/
        template<class T>
        T*                  getDescendantT(const std::string& name, error_policy ep = ep_show_error) { return safeCast<T*>(getDescendant(name, ep)); }
        /**search child by name*/
        spActor             getChild(const std::string& name, error_policy ep = ep_show_error) const;
        /**search child by name and cast it to other class*/
        template<class T>
        T*                  getChildT(const std::string& name, error_policy ep = ep_show_error) const { return safeCast<T*>(getChild(name, ep).get()); }

        /**search tween by name*/
        spTween             getTween(const std::string& name, error_policy ep = ep_show_error);
        /**returns first tween in actor*/
        spTween             getFirstTween() const {return _tweens._first;}
        /**returns last tween in actor*/
        spTween             getLastTween() const {return _tweens._last;}

        const Vector2&      getAnchor() const {return _anchor;}
        float               getAnchorX() const {return _anchor.x;}
        float               getAnchorY() const {return _anchor.y;}
        bool                getIsAnchorInPixels() {return (_flags & flag_anchorInPixels) != 0;}
        const Vector2&      getPosition() const {return _pos;}
        float               getX() const {return _pos.x;}
        float               getY() const {return _pos.y;}
        const Vector2&      getScale() const {return _scale;}
        float               getScaleX() const {return _scale.x;}
        float               getScaleY() const {return _scale.y;}
        /**Returns rotation angle in radians*/
        float               getRotation() const {return _rotation;}
        /**Returns rotation angle in degrees*/
        float               getRotationDegrees() const {return _rotation / MATH_PI * 180.0f;}
        int                 getPriority() const {return _zOrder;}
        bool                getVisible() const {return (_flags & flag_visible) != 0;}
        Actor*              getParent() {return _parent;}
        const Actor*        getParent() const {return _parent;}
        Vector2             getSize() const {return _size;}
        /**Returns Size*Scale*/
        Vector2             getScaledSize() const { return _size.mult(_scale); }
        float               getWidth() const;
        float               getScaledWidth() const {return _size.x * _scale.x;}
        float               getHeight() const;
        float               getScaledHeight() const {return _size.y * _scale.y;}
        unsigned char       getAlpha() const;
        const spClock&      getClock() const;
        virtual RectF       getDestRect() const;
        /**returns touch id if actor is pressed down*/
        pointer_index       getPressed(MouseButton b = MouseButton_Touch) const;
        /**returns touch id if actor is moused overred*/
        pointer_index       getOvered() const;
        bool                getTouchEnabled() const { return (_flags & flag_touchEnabled) != 0; }
        bool                getTouchChildrenEnabled() const { return (_flags & flag_touchChildrenEnabled) != 0; }
        UpdateCallback      getCallbackDoUpdate() const {return _cbDoUpdate;}
        RenderDelegate*     getRenderDelegate() { return _rdelegate; }
        //RenderCallback        getCallbackDoRender() const {return _cbDoRender;}

        /**return local actor transformation*/
        const Transform&      getTransform() const;
        const Transform&      getTransformInvert() const;


        /**computes global actor transformation*/
        Transform           computeGlobalTransform(Actor* parent = 0) const;
        /**computes actor Bounds rectangle. Iterates children*/
        RectF               computeBounds(const Transform& transform = Transform::getIdentity()) const;

        /**Sets Anchor. Anchor also called Pivot point. It is "center" for rotation/scale/position. Anchor could be set in Pixels or in Percents (/100).
        Default value is (0,0) - top left corner of Actor
        */
        void setAnchor(const Vector2& anchor);
        void setAnchor(float ax, float ay);
        void setAnchorInPixels(const Vector2& anchor);
        void setAnchorInPixels(float x, float y);
        void setAnchorX(float x);
        void setAnchorY(float y);

        void setPosition(const Vector2& pos);
        void setPosition(float x, float y);
        void setX(float x);
        void setY(float y);

        /**Overwrites transformation matrix. position/scale/rotation would be ignored until you change them*/
        void setTransform(const AffineTransform& tr);
        /** set z order draw priority, from back (low value) to front (high value). Max value is 32000, Min value -32000*/
        void setPriority(short zorder);
        void setScale(float scale);
        void setScale(const Vector2& scale);
        void setScale(float scaleX, float scaleY);
        void setScaleX(float sx);
        void setScaleY(float sy);
        /**Sets rotation angle in radians*/
        void setRotation(float angle);
        /**Sets rotation angle in degrees. Converts internally to radians. (use setRotation!)*/
        void setRotationDegrees(float angle);

        /**Sets Size of Actor. Size doesn't scale contents of Actor. Size only affects event handling and rendering if you change Anchor*/
        void setSize(const Vector2&);
        void setSize(float w, float h);
        void setWidth(float w);
        void setHeight(float h);
        /**Extends actor's clickable area from each side. Affects only to Actor::isOn. Max value is 127, Min Value is -128*/
        void setExtendedClickArea(char add) {_extendedIsOn = add;}

        void setClock(spClock clock);
        void setRenderDelegate(RenderDelegate* mat);

        /**Show/Hide actor and children. Invisible Actor doesn't receive Touch events.*/
        void setVisible(bool vis) {_flags &= ~flag_visible; if (vis) _flags |= flag_visible;}
        /**Enable/Disable culling this actor outside of clip area (use it in pair with ClipRectActor)*/
        void setCull(bool enable) {_flags &= ~flag_cull; if (enable) _flags |= flag_cull;}
        /**Sets transparency. if alpha is 0 actor and children are completely invisible. Invisible Actor doesn't receive Touch events.*/
        void setAlpha(unsigned char alpha);

        /**Enables/Disables Touch events for Actor.*/
        void setTouchEnabled(bool enabled) { _flags &= ~flag_touchEnabled; if (enabled) _flags |= flag_touchEnabled; }
        /**Enables/Disables Touch events for children of Actor.*/
        void setTouchChildrenEnabled(bool enabled) { _flags &= ~flag_touchChildrenEnabled; if (enabled) _flags |= flag_touchChildrenEnabled; }
        /**setTouchEnabled + setTouchChildrenEnabled*/
        void setTouchEnabled(bool enabled, bool childrenEnabled) { setTouchEnabled(enabled); setTouchChildrenEnabled(childrenEnabled); }

        /**Sets callback which would be called each Actor::update cycle before doUpdate. Use it if you don't want inherit from Actor and overload Actor::doUpdate.*/
        void setCallbackDoUpdate(UpdateCallback cb) {_cbDoUpdate = cb;}
        /**Sets callback which would be called each Actor::render cycle before doRender. Use it if you don't want inherit from Actor and overload Actor::doRender.*/
        //void setCallbackDoRender(RenderCallback cb){_cbDoRender = cb;}

        virtual bool isOn(const Vector2& localPosition, float localScale = 1.0f);
        /**Returns true if actor is child or located deeper in current subtree*/
        bool isDescendant(const spActor& actor) const;

        /**inserts sibling before 'this' actor*/
        void insertSiblingBefore(spActor);
        /**inserts sibling after 'this' actor*/
        void insertSiblingAfter(spActor);
        /**adds child first in list*/
        void prependChild(spActor actor);
        /**adds child first in list*/
        void prependChild(Actor* actor);
        void addChild(spActor actor);
        void addChild(Actor* actor);//avoid conversion to spActor
        void attachTo(spActor parent);
        void attachTo(Actor* parent);//avoid conversion to spActor
        /**Remove one child*/
        void removeChild(spActor actor);
        /**Removes all children from Actor*/
        void removeChildren();


        /**detaches actor from parent and returns parent. return NULL If actor doesn't have parent*/
        Actor* detach();

        /**Dispatches an event into the event flow. The event target is the EventDispatcher object upon which the dispatchEvent() method is called.*/
        void dispatchEvent(Event* event) override;

        virtual void updateStateOvered() {}
        virtual void updateStatePressed() {}

        spTween addTween(spTween);
        spTween addTween2(spTween, const TweenOptions& opt);

        template<class Prop>
        spTween addTween(const Prop& prop, timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, Tween::EASE ease = Tween::ease_linear)
        {return addTween(createTween(prop, duration, loops, twoSides, delay, ease));}

        template<class Prop>
        spTween addTween(const Prop& prop, const TweenOptions& opt)
        {return addTween(createTween2(prop, opt));}

        /**short syntax version of actor->addEventListener(TouchEvent::CLICK, ...);*/
        int addClickListener(const EventCallback& cb) { return addEventListener(TouchEvent::CLICK, cb); }
        /**short syntax version of actor->addEventListener(TouchEvent::TOUCH_DOWN, ...);*/
        int addTouchDownListener(const EventCallback& cb) { return addEventListener(TouchEvent::TOUCH_DOWN, cb); }
        /**short syntax version of actor->addEventListener(TouchEvent::TOUCH_UP, ...);*/
        int addTouchUpListener(const EventCallback& cb) { return addEventListener(TouchEvent::TOUCH_UP, cb); }

        void removeTween(spTween);
        void removeTweensByName(const std::string& name);
        /**remove all tweens and call Tween::complete to them if callComplete == true*/
        void removeTweens(bool callComplete = false);

        /**Updates this actor, children and all tweens.*/
        virtual void update(const UpdateState& us);
        /**Renders this actor and children.*/
        virtual void render(const RenderState& rs);
        virtual void handleEvent(Event* event);
        virtual void doRender(const RenderState& rs) {}

        //converts position in parent space to local space
        Vector2 parent2local(const Vector2& pos) const;
        //converts local position to parent space
        Vector2 local2parent(const Vector2& pos = Vector2(0, 0)) const;

        //converts local position to Stage
        Vector2 local2stage(const Vector2& pos = Vector2(0, 0), Actor* stage = 0) const;
        //converts global position (position in Stage space) to local space
        Vector2 stage2local(const Vector2& pos, Actor* stage = 0) const;

        typedef Property2Args<float, Vector2, const Vector2&, Actor, &Actor::getPosition, &Actor::setPosition>  TweenPosition;
        typedef Property<float, float, Actor, &Actor::getX, &Actor::setX>                                       TweenX;
        typedef Property<float, float, Actor, &Actor::getY, &Actor::setY>                                       TweenY;
        typedef Property<float, float, Actor, &Actor::getWidth, &Actor::setWidth>                               TweenWidth;
        typedef Property<float, float, Actor, &Actor::getHeight, &Actor::setHeight>                             TweenHeight;
        typedef Property2Args2<float, Vector2, Vector2, const Vector2&, Actor, &Actor::getSize, &Actor::setSize>TweenSize;
        typedef Property<float, float, Actor, &Actor::getRotation, &Actor::setRotation>                         TweenRotation;
        typedef Property<float, float, Actor, &Actor::getRotationDegrees, &Actor::setRotationDegrees>           TweenRotationDegrees;
        typedef Property2Args1Arg<float, Vector2, const Vector2&, Actor, &Actor::getScale, &Actor::setScale>    TweenScale;
        typedef Property2Args1Arg<float, Vector2, const Vector2&, Actor, &Actor::getAnchor, &Actor::setAnchor>  TweenAnchor;
        typedef Property<float, float, Actor, &Actor::getScaleX, &Actor::setScaleX>                             TweenScaleX;
        typedef Property<float, float, Actor, &Actor::getScaleY, &Actor::setScaleY>                             TweenScaleY;
        typedef Property<unsigned char, unsigned char, Actor, &Actor::getAlpha, &Actor::setAlpha>               TweenAlpha;


        void serialize(serializedata*) override;
        void deserialize(const deserializedata*) override;

        /**Returns detailed actor information. Used for debug purposes. */
        virtual std::string dump(const dumpOptions& opt) const;

        /**Returns Stage where Actor attached to. Used for multi stage (window) mode*/
        Stage*              _getStage();

        void setNotPressed(MouseButton b);

        bool internalRender(RenderState& rs, const RenderState& parentRS);

        /**recursively removes all event listeners and added tweens*/
        void clean();

        virtual bool getBounds(RectF&) const { return false; }

    protected:

        RenderDelegate* _rdelegate;
        Stage* _stage;

        void added2stage(Stage*);
        void removedFromStage();
        virtual void onAdded2Stage() {}
        virtual void onRemovedFromStage() {}
        virtual void transformUpdated();

        virtual void calcBounds2(RectF& bounds, const Transform& transform) const;


        typedef intrusive_list<spActor> children;
        static void setParent(Actor* actor, Actor* parent);
        static children& getChildren(spActor& actor) { return actor->_children; }
        static unsigned short& _getFlags(Actor* actor) { return actor->_flags; }

        void _onGlobalTouchUpEvent(Event*);
        void _onGlobalTouchUpEvent1(Event*);
        void _onGlobalTouchUpEvent2(Event*);
        void _onGlobalTouchMoveEvent(Event*);

        const Vector2& _getSize() const { return _size; }
        void _setSize(const Vector2&);
        virtual void sizeChanged(const Vector2& size);
        Actor*  _getDescendant(const std::string& name);
        spTween _addTween(spTween tween, bool rel);

        bool prepareRender(RenderState& rs, const RenderState& parentRS);
        void completeRender(const RenderState& rs);


        void markTranformDirty();
        void updateTransform() const;
        void internalUpdate(const UpdateState& us);

        /**doUpdate is virtual method for overloading in inherited classes. UpdateState struct has local time of Actor (relative to Clock) and delta time.*/
        virtual void doUpdate(const UpdateState& us);
        UpdateCallback _cbDoUpdate;

        mutable Transform _transform;
        mutable Transform _transformInvert;


        enum flags
        {
            flag_anchorInPixels         = 1,
            flag_visible                = 1 << 1,
            flag_touchEnabled           = 1 << 2,
            flag_transformDirty         = 1 << 3,
            flag_transformInvertDirty   = 1 << 4,
            flag_touchChildrenEnabled   = 1 << 5,
            flag_cull                   = 1 << 6,
            flag_fastTransform          = 1 << 7,
            flag_boundsNoChildren       = 1 << 8,
            flag_reserved               = 1 << 9,
            flag_last                   = flag_reserved
        };

        mutable unsigned short _flags;
        unsigned char   _alpha;
        char    _extendedIsOn;

        spClock _clock;
        Actor* _parent;

        typedef intrusive_list<spTween> tweens;
        tweens _tweens;

        children _children;

        union
        {
            //dont change order!!! or brake statements: if (_pressedOvered == _overred)
            struct
            {
                pointer_index _overred;
                pointer_index _pressedButton[MouseButton_Num];
            };
            int32_t _pressedOvered;
        };


    private:

        Vector2 _pos;
        Vector2 _anchor;
        Vector2 _scale;
        Vector2 _size;
        float   _rotation;
        short   _zOrder;
    };



    /*Runs callback in time ms.Stage used as default actor*/
    spTween setTimeout(timeMS dur, const EventCallback& cb, Actor* root);
    spTween setTimeout(timeMS dur, const EventCallback& cb, spActor root = 0);

    Vector2 convert_local2stage(spActor child, const Vector2& pos, spActor root = 0);
    Vector2 convert_local2stage(const Actor* child, const Vector2& pos, const Actor* root = 0);
    Vector2 convert_stage2local(spActor child, const Vector2& pos, spActor root = 0);
    Vector2 convert_stage2local(const Actor* child, const Vector2& pos, const Actor* root = 0);


    /*Tests 2 actors intersection and returns contact point in space of object1.*/
    bool testIntersection(spActor obj1, spActor obj2, spActor commonParent = 0, Vector2* contact = 0);


    RectF getActorTransformedDestRect(Actor* actor, const Transform& tr);

    /**changes actor parent but with the same position on the screen*/
    void    reattachActor(spActor actor, spActor newParent, spActor root = 0);

    void decompose(const Transform& t, Vector2& pos, float& angle, Vector2& scale);
    void setDecomposedTransform(spActor& a, const Transform& t);

    /** A TweenDummy class
     *  doing nothing, could be used for calling your callback after timeout
     */
    class TweenDummy
    {
    public:
        typedef Actor type;

        void init(Actor&) {}
        void done(Actor&) {}
        void update(Actor&, float p, const UpdateState& us) {}
    };
}


EDITOR_INCLUDE(Actor);