#pragma  once
#include "core/ref_counter.h"
#include "closure/closure.h"


#ifdef OXYGINE_SDL
typedef struct SDL_Window SDL_Window;
#endif

#define DECLARE_SMART(class_name, spname) \
    class class_name;\
    typedef oxygine::intrusive_ptr<class_name> spname;

#define DECLARENS_SMART(name_space, class_name, spname) \
    namespace name_space \
    {\
        class class_name;\
        typedef oxygine::intrusive_ptr<class_name> spname;\
    }


struct SDL_KeyboardEvent;

namespace pugi
{
    class xml_node;
}
namespace oxygine
{
    namespace text
    {
        class Node;
        struct Symbol;
    }

    template <class T>
    class intrusive_ptr;

    DECLARE_SMART(Actor, spActor);
    DECLARE_SMART(AsyncTask, spAsyncTask);
    DECLARE_SMART(Box9Sprite, spBox9Sprite);
    DECLARE_SMART(Button, spButton);
    DECLARE_SMART(ClipRectActor, spClipRectActor);
    DECLARE_SMART(Clock, spClock);
    DECLARE_SMART(ColorRectSprite, spColorRectSprite);
    DECLARE_SMART(DebugActor, spDebugActor);
    DECLARE_SMART(DragHandler, spDragHandler);
    DECLARE_SMART(EventDispatcher, spEventDispatcher);
    DECLARE_SMART(HttpRequestTask, spHttpRequestTask);
    DECLARE_SMART(InputText, spInputText);
    DECLARE_SMART(MaskedSprite, spMaskedSprite);
    DECLARE_SMART(NativeTexture, spNativeTexture);
    DECLARE_SMART(Object, spObject);
    DECLARE_SMART(Polygon, spPolygon);
    DECLARE_SMART(ProgressBar, spProgressBar);
    DECLARE_SMART(RenderTexture, spRenderTexture);
    DECLARE_SMART(ResAnim, spResAnim);
    DECLARE_SMART(ResBuffer, spResBuffer);
    DECLARE_SMART(ResFont, spResFont);
    DECLARE_SMART(Resource, spResource);
    DECLARE_SMART(STDRenderDelegate, spSTDRenderDelegate);
    DECLARE_SMART(SlidingActor, spSlidingActor);
    DECLARE_SMART(Sprite, spSprite);
    DECLARE_SMART(Stage, spStage);
    DECLARE_SMART(TextField, spTextField);
    DECLARE_SMART(Texture, spTexture);
    DECLARE_SMART(ThreadLoader, spThreadLoader);
    DECLARE_SMART(Tween, spTween);
    DECLARE_SMART(TweenQueue, spTweenQueue);
    DECLARE_SMART(WebImage, spWebImage);
    DECLARE_SMART(Material, spMaterialX);

    class Actor;
    class AnimationFrame;
    class ClipRectActor;
    class Color;
    class ColorRectSprite;
    class CreateResourceContext;
    class DebugActor;
    class Event;
    class EventState;
    class Font;
    class IVideoDriver;
    class Image;
    class ImageData;
    class LoadResourcesContext;
    class MaskedSprite;
    class RenderDelegate;
    class Mem2Native;
    class Mutex;
    class PointerState;
    class PostProcessOptions;
    class ProgressBar;
    class RenderState;
    class ResAnim;
    class ResAtlas;
    class ResFont;
    class ResFontBM;
    class ResStarlingAtlas;
    class Resource;
    class Resources;
    class ResourcesLoadOptions;
    class Restorable;
    class ShaderProgram;
    class SingleResAnim;
    class Sprite;
    class TextField;
    class ThreadDispatcher;
    class TouchEvent;
    class Tween;
    class TweenOptions;
    class UberShaderProgram;
    class UpdateState;
    class VertexDeclaration;
    class XmlWalker;
    struct deserializeLinkData;
    struct deserializedata;
    struct serializedata;

    const int cloneOptionsDoNotCloneClildren = 0x01;
    const int cloneOptionsResetTransform = 0x02;

    typedef Closure<void(Event* ev)> EventCallback;
    typedef Closure<void(const RenderState& rs)> RenderCallback;
    typedef Closure<void(const UpdateState& us)> UpdateCallback;
    typedef int cloneOptions;
    typedef int copyOptions;//deprecated typedef
    typedef int eventType;
    typedef unsigned int dumpOptions;
    typedef size_t glyphOptions;
    typedef void* nativeTextureHandle;


#define DECLARE_COPYCLONE(type) type(const type &src, cloneOptions);\
    virtual type* clone(cloneOptions opt=0) const override {return new type(*this, opt);}\


#define DECLARE_COPYCLONE_NEW(type)  type(const type &src, cloneOptions opt = 0){copyFrom(src, opt);}\
    virtual type* clone(cloneOptions opt=0) const override {type *tp = new type(); tp->copyFrom(*this, opt); return tp;}\
    void copyFrom(const type &src, cloneOptions opt = 0);

#define DECLARE_COPYCLONE_NEW2(type)  type(const type &src, cloneOptions opt = 0);\
    virtual type* clone(cloneOptions opt=0) const override;\
    void copyFrom(const type &src, cloneOptions opt = 0);

#define CREATE_COPYCLONE_NEW(type) type::type(const type &src, cloneOptions opt){copyFrom(src, opt);}\
    type* type::clone(cloneOptions opt) const {type *tp = new type(); tp->copyFrom(*this, opt); return tp;}

}
