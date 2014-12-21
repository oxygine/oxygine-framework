#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "ClipRectActor.h"
#include "Draggable.h"

namespace oxygine
{
	DECLARE_SMART(TreeInspector, spTreeInspector);
	DECLARE_SMART(TreeInspectorPage, spTreeInspectorPage);
	DECLARE_SMART(TreeInspectorLine, spActorPreview);
	DECLARE_SMART(SlidingActor, spSlidingActor);

	class TreeInspectorPreview;
	class ResFont;	
	class Font;
	class ResAnim;
	class Resources;


	/**TreeInspector is visual SceneGraph profiler*/
	class TreeInspector: public ClipRectActor
	{
	public:
		static std::string getDefaultName(){ return "tree_inspector"; }

		TreeInspector();
		~TreeInspector();

		void init(const Vector2 &size, spActor actor);

	protected:		
		static RectF calcBounds(Actor *);
		void render(const RenderState &rs);

		void updateSizes();

		void showPreview(TreeInspectorPreview *item, bool show);		
		void close(Event *ev);
		void wheel(Event *ev);
					
		Resources *_resSystem;

		TreeInspectorPreview *_preview;

		spTreeInspectorPage _rootPage;
		spSlidingActor _sliding;

		friend class TreeInspectorLine;
		friend class TreeInspectorPage;
		friend class TreeInspectorPreview;
	};
}