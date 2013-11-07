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

		void	getViewport(Rect &r) const;
		bool	getScissorRect(Rect &) const;
		const VertexDeclarationGL*	getVertexDeclaration(bvertex_format) const;

		void setScissorRect(const Rect *);
		void setRenderTarget(spNativeTexture rt);

		void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest);
		void setState(STATE, unsigned int value);

	protected:
		unsigned int getPT(IVideoDriver::PRIMITIVE_TYPE pt);
		unsigned int getBT(IVideoDriver::BLEND_TYPE pt);

		void _begin(const Rect &viewport, const Color *clearColor);
		GLint _prevFBO;

		mutable VertexDeclarations<VertexDeclarationGL> _vdeclarations;
	};
}