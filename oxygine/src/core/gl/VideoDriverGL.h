#pragma once
#include "oxygine_include.h"
#include "../VideoDriver.h"
#include "VertexDeclarationGL.h"
#include "oxgl.h"

namespace oxygine
{
	class VertexDeclarationGL;

	/**intermediate gl driver with some shared gl code*/
	class VideoDriverGL: public IVideoDriver
	{
	public:
		VideoDriverGL();


		void	getStats(Stats &s) const;
		void	getViewport(Rect &r) const;
		bool	getScissorRect(Rect &) const;
		const VertexDeclarationGL*	getVertexDeclaration(bvertex_format) const;

		void setScissorRect(const Rect *);
		void setRenderTarget(spNativeTexture rt);

		void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest);
		void setState(STATE, unsigned int value);

		void swapped(){_triangles = 0; _batches = 0;}

	protected:
		unsigned int getPT(IVideoDriver::PRIMITIVE_TYPE pt);
		unsigned int getBT(IVideoDriver::BLEND_TYPE pt);
		void _debugAddPrimitives(IVideoDriver::PRIMITIVE_TYPE pt, int num);
		void setDebugStats(bool enable){_traceStats = enable;}

		void _begin(const Rect &viewport, const Color *clearColor);
		GLint _prevFBO;

		mutable VertexDeclarations<VertexDeclarationGL> _vdeclarations;
		bool _traceStats;
		int _batches;
		int _triangles;
	};
}