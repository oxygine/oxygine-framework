#include "SharedContext.h"
#include <SDL_video.h>
#include "oxygine.h"
#include "gl/oxgl.h"

namespace oxygine
{
    /** Allows the usage of shared OpenGL contexts. Needs to be called before core::init(). */
    void SharedContext::Enable()
    {
        //SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    }

    /** Creates a shared context. Can be called from the main thread. */
    void SharedContext::create()
    {
        OX_ASSERT(_context == NULL);

        SDL_GLContext currentContext = SDL_GL_GetCurrentContext();

        OX_ASSERT(currentContext != NULL);

        _context = SDL_GL_CreateContext( core::getWindow() );

        CHECKGL();

        OX_ASSERT(_context != NULL);

        // restore the previous context
        SDL_GL_MakeCurrent( core::getWindow(), currentContext );
    }

    /** Makes this context the current one. Must be called from within the thread that will use it. */
    void SharedContext::makeCurrent()
    {
        OX_ASSERT(_context != NULL);

        SDL_GL_MakeCurrent( core::getWindow(), _context );

        CHECKGL();
    }

    /** The destructor which releases the context again. */
    SharedContext::~SharedContext()
    {
        if(_context != NULL)
        {
            // disable the context if it is this one
            if( SDL_GL_GetCurrentContext() == _context )
                SDL_GL_MakeCurrent( core::getWindow(), NULL );

            SDL_GL_DeleteContext(_context);

            CHECKGL();

            // just be safe in case of other destructor code accessing this object
            _context = NULL;
        }
    }
}
