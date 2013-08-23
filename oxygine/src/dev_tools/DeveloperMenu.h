#pragma once
#include "ClipRectActor.h"
#include "Draggable.h"

namespace oxygine
{
	DECLARE_SMART(DeveloperMenu, spDeveloperMenu);
	class Resources;

	class DeveloperMenu: public Actor
	{
	public:
		static string getDefaultName(){return "tree_inspector";}

		DeveloperMenu();
		~DeveloperMenu();

		void init(const Vector2 &size, const char *text, spActor data, const Color &color);

		Vector2 getBodySize() const;

	private:
		void close(Event *ev);

		Resources *_resSystem;

		spActor _body;
		Draggable _topDrag;
	};
}