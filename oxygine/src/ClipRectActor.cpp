#include "ClipRectActor.h"
#include "RootActor.h"
#include "core/logging.h"
#include "RenderState.h"

namespace oxygine
{
	ClipRectActor::ClipRectActor(const ClipRectActor &src, cloneOptions opt):Actor(src, opt)
	{
		_culling = src._culling;
		_clipping = src._clipping;
	}

	ClipRectActor::ClipRectActor():_culling(false), _clipping(true)
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
		return Actor::handleEvent(es);
	}
	*/

	void ClipRectActor::handleEvent(Event *event)
	{
		bool touchEvent = event->type > _et_TouchFirst && event->type < _et_TouchLast;
		if (touchEvent)
		{
			TouchEvent *te = safeCast<TouchEvent*>(event);
			Vector2 localPosition = global2local(te->localPosition);		
			if (!isOn(localPosition))
				return;
		}
		
		Actor::handleEvent(event);
	}

	void ClipRectActor::render(const RenderState &parentRS)
	{
		RenderState rs = parentRS;

		Rect scissorRect(0,0,0,0);
		bool scissorEnabled = rs.renderer->getDriver()->getScissorRect(scissorRect);

		bool vis = true;
		if (_clipping)
		{
			parentRS.renderer->drawBatch();
			
			RectF ss_rect = getScreenSpaceDestRect(getTransform() * parentRS.transform);
			parentRS.renderer->drawBatch();
			
			rs.clip.clip(ss_rect);
			if (!rs.clip.isEmpty())
			{
				Rect gl_rect = Rect(
					int(rs.clip.pos.x + 0.01f), 
					int(rs.clip.pos.y + 0.01f), 
					int(rs.clip.size.x + 0.01f), 
					int(rs.clip.size.y + 0.01f));

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
			Actor::render(rs);

		
		if (_clipping)
		{
			parentRS.renderer->drawBatch();

			rs.renderer->getDriver()->setScissorRect(scissorEnabled ? &scissorRect : 0);
		}
	}
}