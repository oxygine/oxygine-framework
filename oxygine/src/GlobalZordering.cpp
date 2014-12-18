#include "GlobalZordering.h"
#include "GraphWalker.h"

namespace oxygine
{
	bool DoCollectActors(Actor* actor, Actor* start, std::vector<Actor*>& actors)
	{
		if(!actor)
			return true;

		if(actor == start)
			return true;

		actors.push_back(actor);
		if(actor->asGlobalZordering() || actor->asLocalZordering())
			return false;

		return true;
	}

	bool CollectActors(Actor* actor, Actor* start, std::vector<Actor*>& actors)
	{
		bool result = DoCollectActors(actor, start, actors);

		if(!result)
			return result;

		spActor child = actor->getFirstChild();

		while(child)
		{
			CollectActors(child.get(), start, actors);
			child = child->getNextSibling();
		}

		return true;
	}

	bool SortFunc(Actor* lhs, Actor* rhs)
	{
		return lhs->getPriority() < rhs->getPriority();
	}

	void GlobalZordering::render(const RenderState &parentRS)
	{
		_parentRenderState = parentRS;

		RenderState rs;
		if(!internalRender(rs, parentRS))
			return;

		if(updateOnlyPass)
		{
			Actor *actor = _children._first.get();
			while(actor)
			{
				OX_ASSERT(actor->getParent());
				actor->render(rs);
				actor = actor->_next.get();
			}

			return;
		}

		std::vector<Actor*> sortedActors;

		CollectActors(this, this, sortedActors);

		std::sort(sortedActors.begin(), sortedActors.end(), SortFunc);

		for(std::vector<Actor*>::iterator it = sortedActors.begin(); it != sortedActors.end(); ++it)
		{
			Actor*& actor = *it;
			if(actor->asGlobalZordering() || actor->asLocalZordering())
				actor->render(actor->getParentRenderState());
			else
				actor->internalRender(RenderState(), actor->getParentRenderState());
		}

	}

	bool GlobalZordering::asGlobalZordering()
	{
		return true;
	}
}
