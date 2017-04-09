#include "ClipRectActor.h"
#include "Stage.h"
#include "core/log.h"
#include "core/oxygine.h"
#include "RenderState.h"
#include "Serialize.h"
#include "Material.h"

namespace oxygine
{
    void ClipRectActor::copyFrom(const ClipRectActor& src, cloneOptions opt)
    {
        inherited::copyFrom(src, opt);
        _clipping = src._clipping;
    }

    ClipRectActor::ClipRectActor(): _clipping(true)
    {

    }

    ClipRectActor::~ClipRectActor()
    {

    }

    void ClipRectActor::handleEvent(Event* event)
    {
        if (TouchEvent::isTouchEvent(event->type))
        {
            TouchEvent* te = safeCast<TouchEvent*>(event);
            Vector2 localPosition = parent2local(te->localPosition);
            if (!isOn(localPosition))
                return;
        }

        inherited::handleEvent(event);
    }

    void ClipRectActor::render(const RenderState& parentRS)
    {
        _rdelegate->render(this, parentRS);
    }

    void ClipRectActor::serialize(serializedata* data)
    {
        inherited::serialize(data);
        pugi::xml_node node = data->node;
        node.set_name("ClipRectActor");
    }

    void ClipRectActor::deserialize(const deserializedata* data)
    {
        inherited::deserialize(data);
    }
}
