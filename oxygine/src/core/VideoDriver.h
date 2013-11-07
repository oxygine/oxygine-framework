#pragma once
#include "oxygine_include.h"
#include "ref_counter.h"
#include "vertex.h"
#include "math/Rect.h"
#include <vector>

namespace oxygine
{	
	class Color;
	DECLARE_SMART(NativeTexture, spNativeTexture);

	class ShaderProgram;
	class VertexDeclaration;

	class IVideoDriver
	{
	public:
		enum PRIMITIVE_TYPE
		{
			PT_POINTS,
			PT_LINES,
			PT_LINE_LOOP,
			PT_LINE_STRIP,
			PT_TRIANGLES,
			PT_TRIANGLE_STRIP,
			PT_TRIANGLE_FAN,
		};

		enum BLEND_TYPE
		{
			BT_ZERO,
			BT_ONE,
			BT_SRC_COLOR,
			BT_ONE_MINUS_SRC_COLOR,
			BT_SRC_ALPHA,
			BT_ONE_MINUS_SRC_ALPHA,
			BT_DST_ALPHA,
			BT_ONE_MINUS_DST_ALPHA,
		};

		enum STATE
		{
			STATE_BLEND,
			STATE_NUM
		};

		static IVideoDriver *instance;
		virtual ~IVideoDriver(){}

		virtual void reset() = 0;
		virtual void restore() = 0;
		virtual bool isReady() const = 0;

		virtual spNativeTexture createTexture() = 0;

		virtual void begin(const Rect &viewport, const Color *color) = 0;				
		virtual void draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl, const void *verticesData, unsigned int numVertices) = 0;
		virtual void draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl, const void *verticesData, unsigned int numVertices, const void *indicesData, unsigned int numIndices, bool indicesShortType) = 0;

		virtual void	getViewport(Rect &r) const = 0;
		virtual bool	getScissorRect(Rect &) const = 0;
		virtual const VertexDeclaration *getVertexDeclaration(bvertex_format) const = 0;
		
		virtual void setScissorRect(const Rect *) = 0;
		virtual void setDefaultSettings() = 0;
		virtual void setRenderTarget(spNativeTexture) = 0;
		virtual void setShaderProgram(ShaderProgram*) = 0;
		virtual void setTexture(int sampler, spNativeTexture) = 0;
		virtual void setState(STATE, unsigned int value) = 0;
		virtual void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest) = 0;
	};		

	class VideoDriverNull: public IVideoDriver
	{
	public:
		spNativeTexture createTexture();

		void begin(const Rect &viewport, const Color *clearColor);
		bool isReady() const {return true;}
		void getViewport(Rect &r) const;
		bool getScissorRect(Rect &) const;
		const VertexDeclaration*	getVertexDeclaration(bvertex_format) const;
		
		void draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl, const void *verticesData, unsigned int verticesDataSize){}
		void draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl, const void *verticesData, unsigned int verticesDataSize, const void *indicesData, unsigned int indicesDataSize, bool indicesShortType){}


		void setScissorRect(const Rect *);
		void setDefaultSettings();
		void setRenderTarget(spNativeTexture);
		void setShaderProgram(ShaderProgram*);
		void setTexture(int sampler, spNativeTexture);
		void setState(STATE, unsigned int value){}
		void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest){}

		void reset(){}
		void restore(){}
	};

}