#pragma once
#include "oxygine_include.h"
#include "math/Vector2.h"
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
    class ThreadMessages;

    void* fastAlloc(size_t size);
    void fastFree(void* data);

    typedef int timeMS;

    /** returns local app time in milliseconds (1sec = 1000ms). Counting starts from zero*/
    timeMS          getTimeMS();

    /** returns UTC time in milliseconds */
    int64           getTimeUTCMS();

    /** is any network connection available?*/
    bool            isNetworkAvaible();

    /**returns locale. ISO 639-1 */
    std::string     getLanguage();


    /**sleep for milliseconds*/
    void    sleep(timeMS);


    namespace core
    {
        struct init_desc
        {
            init_desc() : mode24bpp(true), w(-1), h(-1), fullscreen(false), title("Oxygine"), vsync(true), appName(0), companyName(0), force_gles(false) {}

            /**sets 24 bits per pixel, otherwise sets 16 bits per pixel?*/
            bool mode24bpp;
            /**display width*/
            int w;
            /**display height*/
            int h;
            /**vertical sync*/
            bool vsync;
            /**fullscreen mode*/
            bool fullscreen;
            /**window title*/
            const char* title;

            /** Application name to be used as part of the file system directory for writable storage. If appName is empty files would be written next to working directory*/
            const char* appName;
            /** Company name to be used as part of the file system directory for writable storage*/
            const char* companyName;

            /**use OpenGLES driver. Could be used on Windows for emulation OpenGLES via Direct3D*/
            bool force_gles;
        };

        /** Initializes Oxygine*/
        void init(init_desc* desc = 0);

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

        ThreadMessages& getMainThreadMessages();
        ThreadMessages& getUiThreadMessages();

        bool isActive();
        bool hasFocus();

#ifdef OXYGINE_SDL
        SDL_GLContext   getGLContext();
        SDL_Window*     getWindow();
#endif
    }
}
