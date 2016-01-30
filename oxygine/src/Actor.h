#pragma once
#include "oxygine_include.h"
#include "core/Object.h"
#include "core/Renderer.h"
#include "math/Rect.h"
#include "math/AffineTransform.h"
#include "utils/intrusive_list.h"
#include "Serializable.h"
#include "EventDispatcher.h"
#include "TouchEvent.h"
#include "Tween.h"



namespace oxygine
{
    class Event;
    typedef char pointer_index;

    typedef unsigned int dumpOptions;

    class RenderState;
    class UpdateState;
    class Material;


    DECLARE_SMART(Texture, spTexture);
    DECLARE_SMART(Actor, spActor);
    DECLARE_SMART(Clock, spClock);


    typedef Closure<void (const UpdateState& us)> UpdateCallback;
    typedef Closure<void (const RenderState& rs)> RenderCallback;

    const int cloneOptionsDoNotCloneClildren = 0x01;
    const int cloneOptionsResetTransform = 0x02;
    typedef int cloneOptions;
    typedef int copyOptions;//deprecated typedef


#define DECLARE_COPYCLONE(type) type(const type &src, cloneOptions);\
    virtual type* clone(cloneOptions opt=0) const {return new type(*this, opt);}\


#define DECLARE_COPYCLONE_NEW(type)  type(const type &src, cloneOptions opt = 0){copyFrom(src, opt);}\
    virtual type* clone(cloneOptions opt=0) const {type *tp = new type(); tp->copyFrom(*this, opt); return tp;}\
    virtual void copyFrom(const type &src, cloneOptions opt = 0);

#define DECLARE_COPYCLONE_NEW2(type)  type(const type &src, cloneOptions opt = 0);\
    virtual type* clone(cloneOptions opt=0) const;\
    virtual void copyFrom(const type &src, cloneOptions opt = 0);

#define CREATE_COPYCLONE_NEW(type) type::type(const type &src, cloneOptions opt){copyFrom(src, opt);}\
    type* type::clone(cloneOptions opt) const {type *tp = new type(); tp->copyFrom(*this, opt); return tp;}


    class TweenOptions
    {
    public:
        explicit TweenOptions(timeMS duration = 500) : _duration(duration), _delay(0), _ease(Tween::ease_linear), _loops(1), _twoSides(false), _detach(false) {}
        TweenOptions& duration(timeMS duration) { _duration = duration; return *this; }
        TweenOptions& delay(timeMS delay) { _delay = delay; return *this; }
        TweenOptions& loops(int loops) { _loops = loops; return *this; }
        TweenOptions& twoSides(bool enabled = true) { _twoSides = enabled; return *this; }
        TweenOptions& ease(Tween::EASE ease) { _ease = ease; return *this; }
        TweenOptions& detach(bool detach_ = true) { _detach = detach_; return *this; }

        timeMS  _duration;
        timeMS  _delay;
        Tween::EASE _ease;
        int     _loops;
        bool    _twoSides;
        bool    _detach;
    };

    class Actor : public EventDispatcher, public intrusive_list_item<spActor>, public Serializable
    {
        typedef intrusive_list_item<spActor> intr_list;
    public:

        DECLARE_COPYCLONE_NEW2(Actor);

        Actor();
        virtual ~Actor();

        /**returns first child*/
        spActor             getFirstChild() const {return _children._first;}
        /**returns last child*/
        spActor             getLastChild() const {return _children._last;}
        /**returns next sibling*/
        spActor             getNextSibling() {return intr_list::getNextSibling();}
        /**returns previous sibling*/
        spActor             getPrevSibling() {return intr_list::getPrevSibling();}


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
        pointer_index       getPressed() const;
        /**returns touch id if actor is moused overred*/
        pointer_index       getOvered() const;
        bool                getInputEnabled() const {return (_flags & flag_touchEnabled) != 0;}
        bool                getInputChildrenEnabled() const {return (_flags & flag_touchChildrenEnabled) != 0;}
        bool                getChildrenRelative() const {return (_flags & flag_childrenRelative) != 0;;}
        UpdateCallback      getCallbackDoUpdate() const {return _cbDoUpdate;}
        Material*           getMaterial() { return _material; }
        //RenderCallback        getCallbackDoRender() const {return _cbDoRender;}

        const Transform&      getTransform() const;
        const Transform&      getTransformInvert() const;


        /**Sets Anchor. Anchor is "center" of rotation/scale/position*/
        void setAnchor(const Vector2& anchor);
        void setAnchor(float ax, float ay);
        void setAnchorInPixels(const Vector2& anchor);
        void setPosition(const Vector2& pos);
        void setPosition(float x, float y);
        void setX(float x);
        void setY(float y);
        void setAnchorX(float x);
        void setAnchorY(float y);
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
        /**This option is connected with Anchor. By default value is True*/
        void setChildrenRelative(bool r) {_flags &= ~flag_childrenRelative; if (r) _flags |= flag_childrenRelative;}

        /**Sets Size of Actor. Size doesn't scale contents of Actor. Size only affects event handling and rendering if you change Anchor*/
        void setSize(const Vector2&);
        void setSize(float w, float h);
        void setWidth(float w);
        void setHeight(float h);
        /**Extends actor's clickable area from each side. Affects only to Actor::isOn. Max value is 127, Min Value is -128*/
        void setExtendedClickArea(char add) {_extendedIsOn = add;}

        void setClock(spClock clock);
        void setMaterial(Material* mat);

        /**Show/Hide actor and children. Invisible Actor doesn't receive events.*/
        void setVisible(bool vis) {_flags &= ~flag_visible; if (vis) _flags |= flag_visible;}
        /**Enable/Disable culling this actor outside of clip area (use it with ClipRectActor)*/
        void setCull(bool enable) {_flags &= ~flag_cull; if (enable) _flags |= flag_cull;}
        /**Sets transparency. if alpha is 0 actor and children are completely invisible, don't rendering and don't receive events.*/
        void setAlpha(unsigned char alpha);

        /**Deprecated, use setTouchEnabled*/
        void setInputEnabled(bool enabled) { setTouchEnabled(enabled); }
        /**Deprecated, use setTouchChildrenEnabled*/
        void setInputChildrenEnabled(bool enabled) { setTouchChildrenEnabled(enabled); }

        /**Enables/Disables Touch events for Actor.*/
        void setTouchEnabled(bool enabled) { _flags &= ~flag_touchEnabled; if (enabled) _flags |= flag_touchEnabled; }
        /**Enables/Disables Touch events for children of Actor.*/
        void setTouchChildrenEnabled(bool enabled) { _flags &= ~flag_touchChildrenEnabled; if (enabled) _flags |= flag_touchChildrenEnabled; }

        /**Sets callback which would be called each Actor::update cycle before doUpdate. Use it if you don't want inherit from Actor and overload Actor::doUpdate.*/
        void setCallbackDoUpdate(UpdateCallback cb) {_cbDoUpdate = cb;}
        /**Sets callback which would be called each Actor::render cycle before doRender. Use it if you don't want inherit from Actor and overload Actor::doRender.*/
        //void setCallbackDoRender(RenderCallback cb){_cbDoRender = cb;}

        virtual bool isOn(const Vector2& localPosition);
        /**Returns true if actor is child or located deeper in current subtree*/
        bool isDescendant(spActor actor);

        /**Inserts the specified actor before "where" actor as a child*/
        void insertChildBefore(spActor actor, spActor where);
        /**Inserts the specified actor after "where" actor as a child*/
        void insertChildAfter(spActor actor, spActor where);
        void prependChild(spActor actor);
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
        void dispatchEvent(Event* event);

        virtual void updateState() {}

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
        /**remove all tweens and call completes them if callComplete == true*/
        void removeTweens(bool callComplete = false);

        /**Updates this actor, children and all tweens.*/
        virtual void update(const UpdateState& us);
        /**Renders this actor and children.*/
        virtual void render(const RenderState& rs);
        virtual void handleEvent(Event* event);
        virtual void doRender(const RenderState& rs) {}

        //converts global position (position in parent space) to local space
        Vector2 global2local(const Vector2& pos) const;
        //converts local position to parent space
        Vector2 local2global(const Vector2& pos) const;


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


        void serialize(serializedata*);
        void deserialize(const deserializedata*);

        /**Returns detailed actor information. Used for debug purposes. */
        virtual std::string dump(const dumpOptions& opt) const;

        /**Returns Stage where Actor attached to. Use if for multi stage (window) mode*/
        Stage*              _getStage();

        void setNotPressed();

        bool internalRender(RenderState& rs, const RenderState& parentRS);


    protected:

        Material* _material;
        Stage* _stage;

        void added2stage(Stage*);
        void removedFromStage();
        virtual void onAdded2Stage() {}
        virtual void onRemovedFromStage() {}


        typedef intrusive_list<spActor> children;
        static void setParent(Actor* actor, Actor* parent);
        static children& getChildren(spActor& actor) { return actor->_children; }

        void _onGlobalTouchUpEvent(Event*);
        void _onGlobalTouchMoveEvent(Event*);

        RectF calcDestRectF(const RectF& destRect, const Vector2& size) const;
        void _setSize(const Vector2&);
        virtual void sizeChanged(const Vector2& size);
        Actor*  _getDescendant(const std::string& name);
        spTween _addTween(spTween tween, bool rel);

        bool prepareRender(RenderState& rs, const RenderState& parentRS);
        void completeRender(const RenderState& rs);


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
            flag_childrenRelative       = 1 << 3,
            flag_transformDirty         = 1 << 4,
            flag_transformInvertDirty   = 1 << 5,
            flag_touchChildrenEnabled   = 1 << 6,
            flag_cull                   = 1 << 7,
            flag_fastTransform          = 1 << 8,
            flag_last                   = flag_fastTransform
        };

        mutable unsigned short _flags;
        unsigned char   _alpha;
        char    _extendedIsOn;

        spClock _clock;
        Actor* _parent;

        typedef intrusive_list<spTween> tweens;
        tweens _tweens;

        children _children;

        pointer_index _pressed;
        pointer_index _overred;

    private:
        short   _zOrder;

        Vector2 _pos;
        Vector2 _anchor;
        Vector2 _scale;
        Vector2 _size;
        float   _rotation;
    };

    Vector2 convert_global2local(spActor child, spActor parent, const Vector2& pos);//deprecated, use convert_stage2local
    Vector2 convert_local2global(spActor child, spActor parent, const Vector2& pos);//deprecated, use convert_local2stage

    Vector2 convert_local2stage(spActor child, const Vector2& pos, spActor root = 0);
    Vector2 convert_local2stage(const Actor* child, const Vector2& pos, const Actor* root = 0);
    Vector2 convert_stage2local(spActor child, const Vector2& pos, spActor root = 0);
    Vector2 convert_stage2local(const Actor* child, const Vector2& pos, const Actor* root = 0);

    /**Deprecated*/
    inline Vector2 convert_local2root(spActor child, const Vector2& pos, spActor root = 0) { return convert_local2stage(child, pos, root); }
    /**Deprecated*/
    inline Vector2 convert_root2local(spActor child, const Vector2& pos, spActor root = 0) { return convert_stage2local(child, pos, root); }

    /*Tests 2 actors intersection and returns contact point in space of object1.*/
    bool testIntersection(spActor obj1, spActor obj2, spActor commonParent = 0, Vector2* contact = 0);


    Transform getGlobalTransform(spActor child, spActor parent = 0);
    Transform getGlobalTransform2(spActor child, Actor* parent = 0);
    RectF getActorTransformedDestRect(Actor* actor, const Transform& tr);

    void    changeParentAndSavePosition(spActor mutualParent, spActor actor, spActor newParent);


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


#ifdef OX_EDITOR
#include "EditorActor.h"
#else
namespace oxygine
{
    typedef Actor _Actor;
}
#endif
