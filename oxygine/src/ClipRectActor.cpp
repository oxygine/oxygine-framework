#include "ClipRectActor.h"
#include "Stage.h"
#include "core/log.h"
#include "core/oxygine.h"
#include "RenderState.h"
#include "Serialize.h"

namespace oxygine
{
    void ClipRectActor::copyFrom(const ClipRectActor& src, cloneOptions opt)
    {
        _Actor::copyFrom(src, opt);
        _culling = src._culling;
        _clipping = src._clipping;
    }

    ClipRectActor::ClipRectActor(): _culling(false), _clipping(true)
    {

    }

    ClipRectActor::~ClipRectActor()
    {

    }

    /*
    bool ClipRectActor::handleEvent(const EventState &es)
    {
        if (es.type == Event::EventHandlerDown || es.type == Event::EventHandlerUp || es.type == Event::EventHandlerMotion)
        {
            Vector2 localPosition = global2local(es.localPosition);
            if (!isOn(localPosition))
                return false;
        }
        return _Actor::handleEvent(es);
    }
    */

    void ClipRectActor::handleEvent(Event* event)
    {
        if (TouchEvent::isTouchEvent(event->type))
        {
            TouchEvent* te = safeCast<TouchEvent*>(event);
            Vector2 localPosition = global2local(te->localPosition);
            if (!isOn(localPosition))
                return;
        }

        _Actor::handleEvent(event);
    }

    void ClipRectActor::render(const RenderState& parentRS)
    {
        RenderState rs = parentRS;

        const RectF* parentClippedRect = parentRS.clip;
        RectF clippedRect = *parentClippedRect;
        rs.clip = &clippedRect;

        Rect scissorRect(0, 0, 0, 0);
        bool scissorEnabled = rs.renderer->getDriver()->getScissorRect(scissorRect);

        bool vis = true;
        if (_clipping)
        {
            parentRS.renderer->drawBatch();

            RectF ss_rect = getScreenSpaceDestRect(getTransform() * parentRS.transform);
            parentRS.renderer->drawBatch();

            clippedRect.clip(ss_rect);
            if (!clippedRect.isEmpty())
            {
                Rect gl_rect = Rect(
                                   int(clippedRect.pos.x + 0.01f),
                                   int(clippedRect.pos.y + 0.01f),
                                   int(clippedRect.size.x + 0.01f),
                                   int(clippedRect.size.y + 0.01f));

                Point vp_size = core::getDisplaySize();
                gl_rect.pos.y = vp_size.y - gl_rect.getBottom();

                rs.renderer->getDriver()->setScissorRect(&gl_rect);
            }
            else
            {
                vis = false;
            }

        }

        if (vis)
            _Actor::render(rs);


        if (_clipping)
        {
            parentRS.renderer->drawBatch();

            rs.renderer->getDriver()->setScissorRect(scissorEnabled ? &scissorRect : 0);
        }
    }

    void ClipRectActor::serialize(serializedata* data)
    {
        _Actor::serialize(data);
        pugi::xml_node node = data->node;
        node.set_name("Sprite");
    }

    void ClipRectActor::deserialize(const deserializedata* data)
    {
        _Actor::deserialize(data);
    }
}