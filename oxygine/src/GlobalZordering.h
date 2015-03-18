#pragma once

#include "Actor.h"
#include "RenderState.h"

namespace oxygine
{
	DECLARE_SMART(GlobalZordering, spGlobalZordering)
	class GlobalZordering : public Actor
	{
	public:
		virtual void render(const RenderState &rs);
		virtual bool asGlobalZordering();

	protected:
		bool DoCollectActors(Actor* actor, Actor* start, std::vector<Actor*>& actors);
		bool CollectActors(Actor* actor, Actor* start, std::vector<Actor*>& actors);
	};

}