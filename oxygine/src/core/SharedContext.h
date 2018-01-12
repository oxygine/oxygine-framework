#ifndef SHAREDCONTEXT_H
#define SHAREDCONTEXT_H

typedef void *SDL_GLContext;

namespace oxygine
{
    /** This class creates a shared context that can be used to call OpenGL functions inside of other threads.
     *
     * Usage:
     *
     *   main thread
     *     SharedContext::Enable();
     *
     *     core::init();
     *
     *     mythread.sharedContext.create();
     *
     *   mythread
     *     void run()
     *     {
     *       sharedContext.makeCurrent();
     *
     *       while(true) { }
     *     }
     */

    class SharedContext
    {
    private:

        /** The shared context that was created. */
        SDL_GLContext _context;

    public:

        /** Allows the usage of shared OpenGL contexts. Needs to be called before core::init(). */
        static void Enable();

        /** Creates a shared context object with no OpenGL context being created yet. */
        SharedContext() :
            _context(0)
        {
        }

        /** Returns true when the context was created. Can be used to wait for a thread to create its context. */
        bool created() const { return _context != 0; }

        /** Creates a shared context. Can be called from the main thread. */
        void create();

        /** Makes this context the current one. Must be called from within the thread that will use it. */
        void makeCurrent();

        /** The destructor which releases the context again. */
        ~SharedContext();
    };
}

#endif // SHAREDCONTEXT_H
