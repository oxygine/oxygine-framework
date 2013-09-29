#pragma once
#include "oxygine_include.h"
#include "ref_counter.h"
#include <vector>
#include "math/matrix.h"
#include "math/Rect.h"
#include "vertex.h"

namespace oxygine
{	
	class Color;
	DECLARE_SMART(NativeTexture, spNativeTexture);

	struct vertexP2T2
	{
		float x, y;		
		float u, v;

		Vector2 &getPos() {return *((Vector2*)&x);}
		Vector2 &getUV() {return *((Vector2*)&u);}
	};

	struct vertexPCT2
	{
		float x, y, z;
		unsigned int color;
		float u, v;				
	};

	struct vertexPCT2T2: public vertexPCT2
	{	
		float u2, v2;				
	};

	typedef std::vector<unsigned char> Vertices;

	enum blend_mode
	{
		blend_default,
		blend_disabled,
		blend_premultiplied_alpha,
		blend_alpha,
		blend_add,
		//blend_xor,

		//gles2
		blend_sub
	};

	class UberShaderProgram;
	typedef UberShaderProgram* shaderProgram;
	class VertexDeclaration;
	

	struct batch
	{
		batch();
		~batch();

		//bvertex_format bformat;
		Vertices vertices;
		const VertexDeclaration *vdecl;

		spNativeTexture base;
		spNativeTexture alpha;
		spNativeTexture mask;
		RectF clipMask;
		bool basePremultiplied;

		blend_mode blend;
		shaderProgram program;
	};

	class IVideoDriver
	{
	public:
		static IVideoDriver *instance;
		virtual ~IVideoDriver(){}

		virtual spNativeTexture createTexture() = 0;

		virtual void begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *color) = 0;				
		virtual void drawBatch(const batch &b) = 0;

		virtual void	getViewport(Rect &r) const = 0;
		virtual int		getMaxVertices() const = 0;
		virtual bool	getScissorRect(Rect &) const = 0;
		virtual const VertexDeclaration *getVertexDeclaration(bvertex_format) const = 0;
		
		virtual void setScissorRect(const Rect *) = 0;
		virtual void setDefaultSettings() = 0;
		virtual void setRenderTarget(spNativeTexture) = 0;
	};


	typedef std::vector<unsigned char> indices8;
	typedef std::vector<unsigned short> indices16;
	
	extern indices8 _indices8;
	extern indices16 _indices16;
		

	class VideoDriverNull: public IVideoDriver
	{
	public:
		spNativeTexture createTexture();

		void begin(const Matrix &proj, const Matrix &view, const Rect &viewport, const Color *clearColor);

		void	getViewport(Rect &r) const;
		int		getMaxVertices() const;
		bool	getScissorRect(Rect &) const;
		const VertexDeclaration*	getVertexDeclaration(bvertex_format) const;
		
		void drawBatch(const batch &b);

		void setScissorRect(const Rect *);
		void setDefaultSettings();
		void setRenderTarget(spNativeTexture);
	};

}