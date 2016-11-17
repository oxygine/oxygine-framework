#pragma  once
#include "oxygine-include.h"
#include "core/ref_counter.h"
#include "core/intrusive_ptr.h"
#include "closure/closure.h"


#ifdef OXYGINE_SDL
typedef struct SDL_Window SDL_Window;
#endif

struct SDL_KeyboardEvent;


namespace oxygine
{
    namespace text
    {
        class Node;
        struct Symbol;
    }

    class Event;

    typedef unsigned int dumpOptions;
    class UberShaderProgram;
    class RenderState;
    class UpdateState;
    class Material;
    class ResAnim;
    class EventState;
    class Actor;
    class ResAnim;
    class AnimationFrame;
    class Sprite;
    class UpdateState;
    class PointerState;
    class Resources;
    class Resource;
    class Font;
    class ResFont;
    class ClipRectActor;
    class Sprite;
    class TextField;
    class ColorRectSprite;
    class DebugActor;

    DECLARE_SMART(HttpRequestTask, spHttpRequestTask);
    DECLARE_SMART(Sprite, spSprite);
    DECLARE_SMART(WebImage, spWebImage);


    DECLARE_SMART(Texture, spTexture);
    DECLARE_SMART(Actor, spActor);
    DECLARE_SMART(Clock, spClock);
    DECLARE_SMART(DragHandler, spDragHandler);

    DECLARE_SMART(Actor, spActor);
    DECLARE_SMART(Box9Sprite, spBox9Sprite);
    DECLARE_SMART(Button, spButton);
    DECLARE_SMART(ClipRectActor, spClipRectActor);
    DECLARE_SMART(Clock, spClock);
    DECLARE_SMART(ColorRectSprite, spColorRectSprite);
    DECLARE_SMART(DebugActor, spDebugActor);
    DECLARE_SMART(EventDispatcher, spEventDispatcher);
    DECLARE_SMART(HttpRequestTask, spHttpRequestTask);
    DECLARE_SMART(InputText, spInputText);
    DECLARE_SMART(MaskedSprite, spMaskedSprite);

    class Event;
    class Material;
    class PointerState;
    DECLARE_SMART(Polygon, spPolygon);
    DECLARE_SMART(ProgressBar, spProgressBar);
    DECLARE_SMART(SlidingActor, spSlidingActor);
    DECLARE_SMART(Sprite, spSprite);
    DECLARE_SMART(Stage, spStage);
    DECLARE_SMART(STDMaterial, spSTDMaterial);
    DECLARE_SMART(TextField, spTextField);
    DECLARE_SMART(ThreadLoader, spThreadLoader);
    DECLARE_SMART(Tween, spTween);
    DECLARE_SMART(TweenQueue, spTweenQueue);
    DECLARE_SMART(WebImage, spWebImage);





    class AnimationFrame;

    class Resource;
    class ResAnim;
    class ResFont;

    struct serializedata;
    struct deserializedata;
    struct deserializeLinkData;

    class Color;
    class Event;
    class Material;

    class Actor;
    class ShaderProgram;
    class PostProcessOptions;

    class UpdateState;
    class Tween;
    class Actor;
    class UpdateState;
    class UpdateState;
    class TweenOptions;

    class RenderState;
    class Actor;
    class ClipRectActor;
    class MaskedSprite;
    class Sprite;
    class TextField;
    class ColorRectSprite;
    class ProgressBar;
    DECLARE_SMART(TextField, spTextField);

    DECLARE_SMART(Stage, spStage);
    class Actor;
    class EventState;

    DECLARE_SMART(NativeTexture, spNativeTexture);

    typedef unsigned int glyphOptions;

    typedef int eventType;
    class Event;

    typedef int eventType;
    DECLARE_SMART(EventDispatcher, spEventDispatcher);
    class TouchEvent;

    class ResFontBM;
    class ResAnim;
    class Resources;

    DECLARE_SMART(TextField, spTextField);
    DECLARE_SMART(ColorRectSprite, spColorRectSprite);



    DECLARE_SMART(Texture, spTexture);
    DECLARE_SMART(NativeTexture, spNativeTexture);


    DECLARE_SMART(STDMaterial, spSTDMaterial);
    DECLARE_SMART(AsyncTask, spAsyncTask);

    typedef Closure<void(Event* ev)> EventCallback;


    typedef Closure<void(const UpdateState& us)> UpdateCallback;
    typedef Closure<void(const RenderState& rs)> RenderCallback;
    typedef int cloneOptions;
    typedef int copyOptions;//deprecated typedef


    const int cloneOptionsDoNotCloneClildren = 0x01;
    const int cloneOptionsResetTransform = 0x02;


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

}