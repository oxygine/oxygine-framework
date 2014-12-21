#pragma once
#include "oxygine_include.h"
#include "Actor.h"

namespace oxygine
{
	DECLARE_SMART(ClipRectActor, spClipRectActor);		
	/**
	ClipRectActor clips all out of bound children. Rotation is not supported
	*/
	class ClipRectActor : public Actor
	{
	public:
		DECLARE_COPYCLONE_NEW(ClipRectActor);
		ClipRectActor();
		~ClipRectActor();

		///**Enables culling for children. By default is off*/
		//void setEnableCulling(bool enable){_culling = enable;}

		/**Enables clipping for children. By default is on*/
		void setEnableClipping(bool enable){_clipping = enable;}

		void render(const RenderState &rs);


		void handleEvent(Event *event);

		void serialize(serializedata* data);
		void deserialize(const deserializedata* data);

	protected:

		bool _culling;
		bool _clipping;
	};
}