#pragma once
#include "../oxygine-include.h"
#include "../EventDispatcher.h"
#include "../Event.h"
#include "../math/Vector2.h"
#include <string>

#if OXYGINE_SDL
typedef void* SDL_GLContext;
typedef struct SDL_Window SDL_Window;
typedef SDL_Window* window;
#else
typedef int window;
#endif

/**main oxygine namespace*/
namespace oxygine
{
    class ThreadDispatcher;

    void* fastAlloc(size_t size);
    void fastFree(void* data);

    typedef int timeMS;

    /** returns local app time in milliseconds (1sec = 1000ms). Counting starts from zero*/
    timeMS          getTimeMS();

    /** returns UTC time in milliseconds */
    int64           getTimeUTCMS();

    /** is any network connection available?*/
    bool            isNetworkAvailable();


    int64           getFreeSpace(const char* fullpath = 0);

    /**returns locale. ISO 639-1 */
    std::string     getLanguage();


    /**sleep for milliseconds*/
    void    sleep(timeMS);


    namespace core
    {
        struct init_desc
        {
            init_desc() : w(-1), h(-1),  mode24bpp(true), vsync(true), fullscreen(false), resizable(false), borderless(false), show_window(true), force_gles(false), allow_screensaver(true), title("Oxygine"), appName(0), companyName(0) {}

            /**display width*/
            int w;
            /**display height*/
            int h;

            /**sets 24 bits per pixel, otherwise sets 16 bits per pixel?*/
            bool mode24bpp;
            /**vertical sync*/
            bool vsync;
            /**fullscreen mode*/
            bool fullscreen;
            /**can the window be resized*/
            bool resizable;
            /**borderless window*/
            bool borderless;
            /**will the window be visible*/
            bool show_window;
            /**use OpenGLES driver. Could be used on Windows for emulation OpenGLES via Direct3D*/
            bool force_gles;
            /**allow screensaver*/
            bool allow_screensaver;

            /**window title*/
            const char* title;

            /** Application name to be used as part of the file system directory for writable storage. If appName is empty files would be written next to working directory*/
            const char* appName;
            /** Company name to be used as part of the file system directory for writable storage*/
            const char* companyName;
        };

        void init0();

        /** Initializes Oxygine*/
        int init(init_desc* desc = 0);

        /** Releases all internal components*/
        void release();

        /**sends QUIT event to queue*/
        void requestQuit();

        /**destroy and reset any GPU allocated memory and handles. Call it to free memory if app was minimized (lost focus)*/
        void reset();

        /**restores GPU memory state after reset*/
        void restore();

        /** Update engine*/
        bool update();

        /**returns True if device is ready for rendering*/
        bool isReady2Render();
        /**returns True if device is ready for rendering*/
        bool beginRendering(window i = 0);

        /** Swap Video buffers*/
        void swapDisplayBuffers(window i = 0);

        /**Opens browser*/
        void execute(const char* url);

        /**returns app package, example: com.company.apps*/
        std::string getPackage();

        /** Returns display size in pixels*/
        Point getDisplaySize();

        ThreadDispatcher& getMainThreadDispatcher();
        ThreadDispatcher& getUiThreadMessages();

        bool isActive();
        bool hasFocus();

        bool isMainThread();


#ifdef OXYGINE_SDL
        SDL_GLContext   getGLContext();
        SDL_Window*     getWindow();
#endif

        enum
        {
            EVENT_SYSTEM = sysEventID('c', 'S', 'y'), //events from SDL
            EVENT_PRECREATEWINDOW = sysEventID('c', 'P', 'W'),//dispatched before creating window/context
            EVENT_EXIT = sysEventID('c', 'E', 'x'), //dispatched from core::release
        };

        class PreCreateWindowEvent : public Event
        {
        public:
            PreCreateWindowEvent(): Event(EVENT_PRECREATEWINDOW) {}
            int flags;
        };

        spEventDispatcher getDispatcher();

        void init0();
    }
}
