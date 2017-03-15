#pragma once
#include "oxygine_include.h"
#include "Event.h"
namespace oxygine
{
    class EmscSyncFsEvent: public Event
    {
    public:
        enum
        {
            EVENT = sysEventID('E', 'S', 'F')
        };
    };


    /**will dispatch EmscSyncFsEvent to core::dispatcher*/
    void emscSyncFS(bool read);
}