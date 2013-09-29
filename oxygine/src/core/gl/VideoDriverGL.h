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
		void	getViewport(Rect &r) const;
		int		getMaxVertices() const;
		bool	getScissorRect(Rect &) const;
		const VertexDeclarationGL*	getVertexDeclaration(bvertex_format) const;

		void setScissorRect(const Rect *);
		void setRenderTarget(spNativeTexture rt);

	protected:
		void _begin(const Rect &viewport, const Color *clearColor);

		mutable VertexDeclarations<VertexDeclarationGL> _vdeclarations;
	};
}