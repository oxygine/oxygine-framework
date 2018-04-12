#include "oxygine.h"
#include "Mutex.h"
#include "STDFileSystem.h"
#include "ThreadDispatcher.h"
#include "VideoDriver.h"
#include "log.h"
#include "../Image.h"
#include "../Input.h"
#include "../InputText.h"
#include "../KeyEvent.h"
#include "../MaterialCache.h"
#include "../PointerState.h"
#include "../PostProcess.h"
#include "../STDRenderDelegate.h"
#include "../actor/DebugActor.h"
#include "../actor/Stage.h"
#include "../actor/TextField.h"
#include "../res/CreateResourceContext.h"
#include "../res/ResAtlas.h"
#include "../res/ResBuffer.h"
#include "../res/ResFontBM.h"
#include "../res/ResStarlingAtlas.h"
#include "../res/Resources.h"
#include "../winnie_alloc/winnie_alloc.h"
#include "gl/VideoDriverGLES20.h"
#include "gl/oxgl.h"
#include <stdio.h>

#ifdef EMSCRIPTEN
#include <sys/time.h>
#include <emscripten.h>
//#include <SDL.h>
//#include <SDL_compat.h>
#include <SDL_events.h>
#elif __ANDROID__
#include "../core/android/jniUtils.h"
#elif __APPLE__
#include <TargetConditionals.h>
#include "ios/ios.h"
#endif

#include "pthread.h"

#ifdef OXYGINE_SDL
extern "C"
{
#include "SDL.h"
#include "SDL_video.h"
};

#include <stdio.h>
#include "SDL_syswm.h"
#include "SDL_timer.h"

#endif


#ifdef EMSCRIPTEN
#   define HANDLE_FOCUS_LOST 0

void emscStackTrace()
{
    EM_ASM(
    {
        console.log(new Error().stack);
        /*
        try {
            throw new Error();
        } catch (e) {
            print(e.stack);
        }
        */
    });
}

#elif !SDL_VIDEO_OPENGL
#   define HANDLE_FOCUS_LOST 1
#else
#   define HANDLE_FOCUS_LOST 0
#endif

#define LOST_RESET_CONTEXT 0



#if defined(__arm__)
#if defined(__ARM_ARCH_7A__)
#if defined(__ARM_NEON__)
#if defined(__ARM_PCS_VFP)
#define ABI "armeabi-v7a/NEON (hard-float)"
#else
#define ABI "armeabi-v7a/NEON"
#endif
#else
#if defined(__ARM_PCS_VFP)
#define ABI "armeabi-v7a (hard-float)"
#else
#define ABI "armeabi-v7a"
#endif
#endif
#else
#define ABI "armeabi"
#endif
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define ABI "mips64"
#elif defined(__mips__)
#define ABI "mips"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
#define ABI "unknown"
#endif

namespace oxygine
{
    static ThreadDispatcher _threadMessages;
    static ThreadDispatcher _uiMessages;
    Mutex mutexAlloc;

    bool _useTouchAPI = false;

    bool _renderEnabled = true;

#ifdef OXYGINE_SDL
    static SDL_Window* _window = 0;
    static SDL_GLContext _context = 0;
#endif

    static core::init_desc desc;
    Point _qtFixedSize(0, 0);

    spEventDispatcher _dispatcher;

    static pthread_t _mainThread;

    namespace key
    {
        void update();
    }

    namespace core
    {
        static bool active = true;
        static bool focus = true;

        void focusLost()
        {
            mc().removeUnused();

            if (!LOST_RESET_CONTEXT)
                return;

#if OXYGINE_SDL
            logs::messageln("focus lost");
            SDL_GL_DeleteContext(_context);
            _context = 0;
#endif
        }

        void focusAcquired()
        {
            if (!LOST_RESET_CONTEXT)
                return;

#if OXYGINE_SDL
            logs::messageln("lost context");
            if (!_context)
            {
                _context = SDL_GL_CreateContext(_window);
                initGLExtensions(SDL_GL_GetProcAddress);
            }
#endif
        }

        bool isMainThread()
        {
#ifdef OX_NO_MT
            return true;
#else
            return pthread_equal(_mainThread, pthread_self()) != 0;
#endif
        }

#ifdef EMSCRIPTEN
        void SDL_handleEvent(SDL_Event& event, bool& done);
        int SDL_eventsHandler(void*, SDL_Event* e)
        {
            bool done = false;
            SDL_handleEvent(*e, done);
            return 0;
        }
#endif



        void updateUIMessages()
        {
            ThreadDispatcher::peekMessage msg;
            while (_uiMessages.peek(msg, true)) {}
        }

        void init2();

        void init0()
        {
            if (!_dispatcher)
                _dispatcher = new EventDispatcher;
        }

        int init(init_desc* desc_ptr)
        {
            std::string t;

#ifdef OX_DEBUG
            t += "OX_DEBUG ";
#endif

#ifdef NDEBUG
            t += "NDEBUG ";
#endif

#ifdef _DEBUG
            t += "_DEBUG ";
#endif

#ifdef DEBUG
            t += "DEBUG ";
#endif

            t += "arch " + std::string(ABI);

            logs::messageln("build settings %s", t.c_str());

            init0();

            logs::messageln("initialize oxygine");
            if (desc_ptr)
                desc = *desc_ptr;


            focus = true;
            active = true;


#ifdef OXYGINE_SDL

            logs::messageln("SDL build");


            SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, desc.allow_screensaver ? "1" : "0");

            SDL_Init(SDL_INIT_VIDEO);


            if (desc.mode24bpp)
            {
                SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
                SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
                SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            }
            else
            {
                SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
                SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
                SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
            }

            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
            //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
            //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

            SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);

            if (desc.force_gles)
            {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
            }

            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

            int flags = SDL_WINDOW_OPENGL;

#if TARGET_OS_IPHONE
            flags |= SDL_WINDOW_SHOWN;
            flags |= SDL_WINDOW_BORDERLESS;
            flags |= SDL_WINDOW_ALLOW_HIGHDPI;
#elif __ANDROID__
            flags |= SDL_WINDOW_RESIZABLE;//fix for latest SDL, related with new setOrientationBis java function
#else
            if (desc.show_window)
                flags |= SDL_WINDOW_SHOWN;

            if (desc.borderless)
                flags |= SDL_WINDOW_BORDERLESS;

            if (desc.resizable)
                flags |= SDL_WINDOW_RESIZABLE;
#endif

            //SDL_DisplayMode mode;
            //SDL_GetCurrentDisplayMode(0, &mode);
            //logs::messageln("display mode: %d %d", mode.w, mode.h);

            if (desc.w == -1 && desc.h == -1)
            {
                desc.w = 960;
                desc.h = 640;
            }

            if (desc.fullscreen)
                flags |= SDL_WINDOW_FULLSCREEN;


            PreCreateWindowEvent ev;
			ev.flags = flags;
            _dispatcher->dispatchEvent(&ev);

			flags = ev.flags;


#if TARGET_OS_IPHONE
            //ios bug workaround
            //flags &= ~SDL_WINDOW_FULLSCREEN;
#endif

#if TARGET_OS_IPHONE || defined(__ANDROID__)
            desc.w = -1;
            desc.h = -1;
#endif

            logs::messageln("creating window %d %d", desc.w, desc.h);

            _window = SDL_CreateWindow(desc.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, desc.w, desc.h, flags);

            if (!_window)
            {
                logs::error("can't create window: %s", SDL_GetError());
#ifdef __ANDROID__
                jniRestartApp();
#endif
                return -1;
            }
            _context = SDL_GL_CreateContext(_window);
            if (!_context)
            {
                logs::error("can't create gl context: %s", SDL_GetError());
#ifdef __ANDROID__
                jniRestartApp();
#endif
                return -1;
            }

            SDL_GL_SetSwapInterval(desc.vsync ? 1 : 0);

#ifdef EMSCRIPTEN
            SDL_SetEventFilter(SDL_eventsHandler, 0);

            int v = EM_ASM_INT(
            {
                var p = navigator.platform;
                if (p == 'iPad' || p == 'iPhone' || p == 'iPod')
                    return 1;
                return 0;
            }, 0);

            if (v)
                _useTouchAPI = true;

#endif

#if __ANDROID__ || TARGET_OS_IPHONE
            //if (SDL_GetNumTouchDevices() > 0)
            _useTouchAPI = true;
#endif

#endif

            init2();

            return 1;
        }


        void init2()
        {

#ifndef OX_NO_MT
            _mainThread = pthread_self();
#endif

            if (!_dispatcher)
                _dispatcher = new EventDispatcher;

#ifdef OXYGINE_EDITOR
            //setlocale(LC_ALL, "POSIX");
#endif

            file::init(desc.companyName, desc.appName);

#ifdef OXYGINE_SDL
            int missing = initGLExtensions(SDL_GL_GetProcAddress);
#endif

            Point size = getDisplaySize();
            logs::messageln("display size: %d %d", size.x, size.y);

            IVideoDriver::instance = new VideoDriverGLES20();


            CHECKGL();

            IVideoDriver::instance->setDefaultSettings();

            CHECKGL();

			rsCache().setDriver(IVideoDriver::instance);


            STDRenderer::initialize();

            Resources::registerResourceType(ResAtlas::create, "atlas");
            Resources::registerResourceType(ResBuffer::create, "buffer");
            Resources::registerResourceType(ResFontBM::create, "font");
            Resources::registerResourceType(ResFontBM::createBM, "bmfc_font");
            Resources::registerResourceType(ResFontBM::createSD, "sdfont");
            Resources::registerResourceType(ResStarlingAtlas::create, "starling");

            STDRenderer::instance = new STDRenderer;
            STDRenderDelegate::instance = new STDRenderDelegate;
            Material::null       = new NullMaterialX;
            Material::current = Material::null;

            STDRenderer::current = STDRenderer::instance;

            CHECKGL();

#ifdef OX_DEBUG
#ifndef OXYGINE_EDITOR
            DebugActor::initialize();
            TextField::setDefaultFont(DebugActor::resSystem->getResFont("system"));
#endif
#endif
            logs::messageln("oxygine initialized");
        }

#if OXYGINE_SDL
        Vector2 convertTouch(SDL_Event& ev)
        {
            Point size = getDisplaySize();
            return Vector2(ev.tfinger.x * size.x, ev.tfinger.y * size.y);
        }
#endif



        bool isActive()
        {
            return active;
        }

        bool hasFocus()
        {
            return focus;
        }

        spEventDispatcher getDispatcher()
        {
            return _dispatcher;
        }


#if OXYGINE_SDL
        SDL_GLContext getGLContext()
        {
            return _context;
        }

        SDL_Window* getWindow()
        {
            return _window;
        }
#endif

        void reset()
        {
            logs::messageln("core::reset()");
            clearPostProcessItems();
            Restorable::releaseAll();
            PostProcess::freeShaders();
            STDRenderer::reset();
            IVideoDriver::instance->reset();
            logs::messageln("core::reset() done");
        }

        void restore()
        {
            logs::messageln("core::restore()");
            IVideoDriver::instance->restore();
            STDRenderer::restore();
            Restorable::restoreAll();
            logs::messageln("core::restore() done");
        }

        bool isReady2Render()
        {
            if (!_renderEnabled)
                return false;
            return STDRenderer::isReady();
        }

        bool  beginRendering(window w)
        {
            if (!_renderEnabled)
                return false;

#ifdef OXYGINE_SDL
            SDL_Window* wnd = w;
            if (!wnd)
            {
                if (!focus)
                {
                    //logs::messageln("!focus");
                    return false;
                }

                wnd = _window;
            }
            SDL_GL_MakeCurrent(wnd, _context);
#endif

            CHECKGL();

            bool ready = STDRenderer::isReady();
            if (ready)
            {
                rsCache().reset();
                IVideoDriver::_stats.start = getTimeMS();
                updatePortProcessItems();
                rsCache().reset();
            }
            else
            {
                logs::messageln("!ready");
            }

            return ready;
        }

        void swapDisplayBuffers(window w)
        {
#if OXYGINE_SDL
            if (!_context)
                return;
#endif

            CHECKGL();

#ifdef OXYGINE_SDL
            SDL_Window* wnd = w;
            if (!wnd)
                wnd = _window;
            if (_context)
            {
                int status = SDL_GL_MakeCurrent(wnd, _context);
                if (status)
                {
                    logs::error("SDL_GL_MakeCurrent(): %s", SDL_GetError());
                }
                SDL_GL_SwapWindow(wnd);
            }
#endif
            CHECKGL();

            IVideoDriver::_stats.duration = getTimeMS() - IVideoDriver::_stats.start;
            //sleep(1000/50);
        }

#ifdef OXYGINE_EDITOR
#else
        spStage getStageByWindow(Uint32 windowID)
        {
            SDL_Window* wnd = SDL_GetWindowFromID(windowID);
            if (!wnd)
                return getStage();

            spStage stage = Stage::getStageFromWindow(wnd);
            if (!stage)
                return getStage();

            return stage;
        }

        void SDL_handleEvent(SDL_Event& event, bool& done)
        {
            Input* input = &Input::instance;




            Event ev(EVENT_SYSTEM);
            ev.userData = &event;
            _dispatcher->dispatchEvent(&ev);

            if (!ev.stopsPropagation)
                switch (event.type)
                {
                    case SDL_QUIT:
                        logs::messageln("SDL_QUIT");
                        done = true;
                        break;
                    case SDL_WINDOWEVENT:
                    {
                        /*
                        if (event.window.event == SDL_WINDOWEVENT_ENTER)
                        active = false;
                        if (event.window.event == SDL_WINDOWEVENT_LEAVE)
                        active = true;
                        */

                        logs::messageln("SDL_WINDOWEVENT %d", (int)event.window.event);

                        if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                            active = false;
                        if (event.window.event == SDL_WINDOWEVENT_RESTORED)
                            active = true;

                        bool newFocus = focus;
                        if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                            newFocus = false;
                        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                            newFocus = true;
#ifdef __ANDROID__
                        //if (event.window.event == SDL_WINDOWEVENT_ENTER)
                        //   newFocus = true;
#endif

                        if (focus != newFocus)
                        {
#if HANDLE_FOCUS_LOST
                            focus = newFocus;

                            if (focus)
                                focusAcquired();

                            logs::messageln("focus: %d", (int)focus);
                            Event ev(focus ? Stage::ACTIVATE : Stage::DEACTIVATE);

                            spStage stage = getStageByWindow(event.window.windowID);
                            if (stage)
                                stage->dispatchEvent(&ev);

                            mc().clear();

                            if (!focus)
                                focusLost();
#endif
                        }
                        //logs::messageln("SDL_SYSWMEVENT %d", (int)event.window.event);
                        break;
                    }
                    case SDL_MOUSEWHEEL:
                        input->sendPointerWheelEvent(getStageByWindow(event.window.windowID), Vector2((float)event.wheel.x, (float)event.wheel.y), &input->_pointerMouse);
                        break;
                    case SDL_KEYDOWN:
                    {
                        KeyEvent ev(KeyEvent::KEY_DOWN, &event.key);
                        getStageByWindow(event.window.windowID)->dispatchEvent(&ev);
                    } break;
                    case SDL_KEYUP:
                    {
                        KeyEvent ev(KeyEvent::KEY_UP, &event.key);
                        getStageByWindow(event.window.windowID)->dispatchEvent(&ev);
                    } break;

                    case SDL_MOUSEMOTION:
                        if (!_useTouchAPI)
                            input->sendPointerMotionEvent(getStageByWindow(event.window.windowID), (float)event.motion.x, (float)event.motion.y, 1.0f, &input->_pointerMouse);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                    {
                        if (!_useTouchAPI)
                        {
                            MouseButton b = MouseButton_Left;
                            switch (event.button.button)
                            {
                                case 1: b = MouseButton_Left; break;
                                case 2: b = MouseButton_Middle; break;
                                case 3: b = MouseButton_Right; break;
                            }

                            input->sendPointerButtonEvent(getStageByWindow(event.window.windowID), b, (float)event.button.x, (float)event.button.y, 1.0f,
                                                          event.type == SDL_MOUSEBUTTONDOWN ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP, &input->_pointerMouse);
                        }
                    }
                    break;
                    case SDL_FINGERMOTION:
                    {
#ifdef EMSCRIPTEN
                        _useTouchAPI = true;
#endif

                        if (_useTouchAPI)
                        {
                            //logs::messageln("SDL_FINGERMOTION");
                            Vector2 pos = convertTouch(event);
                            PointerState* ps = input->getTouchByID((int64_t)event.tfinger.fingerId);
                            if (ps)
                                input->sendPointerMotionEvent(getStageByWindow(event.window.windowID),
                                                              pos.x, pos.y, event.tfinger.pressure, ps);
                        }
                    }

                    break;
                    case SDL_FINGERDOWN:
                    case SDL_FINGERUP:
                    {
#ifdef EMSCRIPTEN
                        _useTouchAPI = true;
#endif
                        if (_useTouchAPI)
                        {
                            //logs::messageln("SDL_FINGER");
                            Vector2 pos = convertTouch(event);
                            PointerState* ps = input->getTouchByID((int64_t)event.tfinger.fingerId);
                            if (ps)
                                input->sendPointerButtonEvent(getStageByWindow(event.window.windowID),
                                                              MouseButton_Touch,
                                                              pos.x, pos.y, event.tfinger.pressure,
                                                              event.type == SDL_FINGERDOWN ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP,
                                                              ps);
                        }
                    }
                    break;
                }

        }
#endif

        bool update()
        {
#ifndef OXYGINE_EDITOR
            key::update();
#endif
            timeMS duration = IVideoDriver::_stats.duration;
            IVideoDriver::_stats = IVideoDriver::Stats();
            IVideoDriver::_stats.duration = duration;

            ThreadDispatcher::peekMessage msg;
            while (_threadMessages.peek(msg, true)) {}

#if OXYGINE_SDL

            //logs::messageln("update");

            bool done = false;
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
#if !EMSCRIPTEN //emscripten build handles events from EventsFilter
                SDL_handleEvent(event, done);
#endif
            }

            return done;
#endif

            return true;
        }

        void release()
        {
            logs::messageln("core::release");

#ifndef OXYGINE_EDITOR
            InputText::stopAnyInput();
#endif

            rsCache().reset();
			rsCache().setDriver(0);
            _threadMessages.clear();
            _uiMessages.clear();

            clearPostProcessItems();
            PostProcess::freeShaders();

            Event ev(EVENT_EXIT);
            _dispatcher->dispatchEvent(&ev);

            mc().clear();

            STDRenderer::release();
            delete STDRenderDelegate::instance;
            STDRenderDelegate::instance = 0;

            delete IVideoDriver::instance;
            IVideoDriver::instance = 0;

            Input::instance.cleanup();


            DebugActor::release();

            if (Stage::instance)
                Stage::instance->cleanup();
            Stage::instance = 0;
            file::free();


            Resources::unregisterResourceType("atlas");
            Resources::unregisterResourceType("buffer");
            Resources::unregisterResourceType("font");
            Resources::unregisterResourceType("bmfc_font");
            Resources::unregisterResourceType("sdfont");
            Resources::unregisterResourceType("starling");



#if OXYGINE_SDL
            SDL_GL_DeleteContext(_context);
            SDL_DestroyWindow(_window);
            SDL_Quit();
#endif

			_dispatcher->removeAllEventListeners();
            _dispatcher = 0;
        }

        void execute(const char* str)
        {
#ifdef OXYGINE_EDITOR
#elif __ANDROID__
            jniBrowse(str);
#elif EMSCRIPTEN
            EM_ASM_INT(
            {
                var url = Pointer_stringify($0);
                window.open(url, '_blank');
            }, str);
#elif __APPLE__
            iosNavigate(str);
#else
            OX_ASSERT(!"execute not implemented");
#endif
        }

        std::string getPackage()
        {
#ifdef __ANDROID__
            return jniGetPackage();
#else
            return "unknown";
#endif
        }

        void requestQuit()
        {
            logs::messageln("requestQuit");
#ifdef OXYGINE_SDL
            SDL_Event ev;
            ev.type = SDL_QUIT;
            SDL_PushEvent(&ev);
#endif
        }

        ThreadDispatcher& getMainThreadDispatcher()
        {
            return _threadMessages;
        }

        ThreadDispatcher& getUiThreadMessages()
        {
            return _uiMessages;
        }


        Point getDisplaySize()
        {
#ifdef OXYGINE_EDITOR
            return _qtFixedSize;
#elif OXYGINE_SDL
            int w = 0;
            int h = 0;

            SDL_GL_GetDrawableSize(_window, &w, &h);
            return Point(w, h);
#else
            logs::warning("getDisplaySize not implemented");
            return Point(0, 0);
#endif
        }
    }

    void handleErrorPolicy(error_policy ep, const char* format, ...)
    {
        va_list args;
        va_start(args, format);

        switch (ep)
        {
            case ep_show_error:
                logs::error_va(format, args);
                OX_ASSERT_NL(!"handleErrorPolicy error.");
                break;
            case ep_show_warning:
                logs::warning_va(format, args);
                break;
            case ep_ignore_error:
                break;
            default:
                OX_ASSERT(!"not implemented");
        }

        va_end(args);
    }

    bool    isNetworkAvailable()
    {
#ifdef __ANDROID__
        return jniIsNetworkAvailable();
#endif
        return true;
    }

    int64 getFreeSpace(const char* fullpath /*= 0*/)
    {
#ifdef __ANDROID__
        if (fullpath)
            return jniGetFreeSpace(fullpath);
        return jniGetFreeSpace(file::wfs().getFullPath("").c_str());
#elif __APPLE__
        return iosGetFreeDiskspace();
#endif

        return std::numeric_limits<int64>::max();
    }

    std::string     getLanguage()
    {
#ifdef __ANDROID__
        return jniGetLanguage();
#endif
        return "en";
    }

    void    sleep(timeMS time)
    {
#ifdef OXYGINE_SDL
        SDL_Delay(time);
#else
        logs::warning("sleep not implemented");
#endif
    }
}
