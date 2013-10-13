#pragma once
#include "oxygine_include.h"
#include <vector>

#include "oxygine.h"
#include "Texture.h"
#include "math/Color.h"
#include "math/Rect.h"
#include "math/AffineTransform.h"
#include "NativeTexture.h"
#include "VideoDriver.h"

namespace oxygine
{
	using namespace std;

	class ClipUV
	{
	public:
		ClipUV(){}
		ClipUV(const Vector2 &a, const Vector2 &b, const Vector2 &c, 
			const Vector2 &a_uv, const Vector2 &b_uv, const Vector2 &c_uv):_a(a), _uvA(a_uv)
		{
			_dAB = b - _a;
			_dAC = c - _a;
			_duvAB = b_uv - _uvA;
			_duvAC = c_uv - _uvA;
			_v = 1.0f / (_dAB.x * _dAC.y - _dAB.y * _dAC.x);
		}

		Vector2 calc(const Vector2 &pos) const 
		{
			Vector2 offset = pos - _a;

			float a = (_dAC.y * offset.x - _dAC.x * offset.y) * _v;
			float b = (_dAB.x * offset.y - _dAB.y * offset.x) * _v;

			float u = _uvA.x + _duvAB.x * a + _duvAC.x * b;
			float v = _uvA.y + _duvAB.y * a + _duvAC.y * b;

			return Vector2(u, v);
		}

	protected:
		Vector2 _a;
		Vector2 _uvA;
		float _v;

		Vector2 _dAB;
		Vector2 _dAC;
		Vector2 _duvAB;
		Vector2 _duvAC;
	};
	
	RectF calcUV(const Rect &srcRect, float iw, float ih);

	template<class V>
	void fillQuadT(V *pv, const RectF &srcRect, const RectF &destRect, const AffineTransform &transform, const Color &color)
	{
		float u = srcRect.pos.x;
		float v = srcRect.pos.y;

		float du = srcRect.size.x;
		float dv = srcRect.size.y;	

		V vt;
		vt.color = color.rgba();

		const Vector2 &pos = destRect.pos;
		const Vector2 &size = destRect.size;

		Vector2 p1(pos.x, pos.y);
		Vector2 p2(pos.x, pos.y + size.y);
		Vector2 p3(pos.x + size.x, pos.y);
		Vector2 p4(pos.x + size.x, pos.y + size.y);
		

		p1 = transform.transform(p1);
		p2 = transform.transform(p2);
		p3 = transform.transform(p3);
		p4 = transform.transform(p4);

		vt.z = 0;

		vt.x = p1.x;
		vt.y = p1.y;				
		vt.u = u;
		vt.v = v;
		*pv = vt;
		++pv;

		vt.x = p2.x;
		vt.y = p2.y;		
		vt.u = u;
		vt.v = v + dv;
		*pv = vt;
		++pv;

		vt.x = p3.x;
		vt.y = p3.y;		
		vt.u = u + du;
		vt.v = v;
		*pv = vt;
		++pv;

		vt.x = p4.x;
		vt.y = p4.y;		
		vt.u = u + du;
		vt.v = v + dv;
		*pv = vt;
		++pv;
	} 

	class IVideoDriver;

	class Diffuse;

	class Renderer
	{
	public:
		typedef AffineTransform transform;

		class Stats
		{
		public:
			Stats():batches(0), triangles(0){}
			int batches;
			int triangles;
		};

		/**Sets default rendering opengl options for 2D*/
		static void setDefaultSettings();
		/**Initializes internal classes. Called automatically from oxygine::init();*/
		static void initialize();
		/**Cleans internal static classes*/
		static void release();
		/**White 4x4 Texture*/
		static spNativeTexture white;
				
		static Stats statsPrev;
		static Stats statsCurrent;


		Renderer(IVideoDriver *driver = 0);
		virtual ~Renderer();

		/**Begins rendering into RenderTexture or into primary framebuffer if rt is null*/
		bool begin(spNativeTexture rt, const Rect &viewport, const Color *clearColor);
		/**Completes started rendering and restores previous Framebuffer.*/
		void end();
		/**Sets View matrix*/
		void setViewTransform(const Matrix &m);
		/**Sets Projection matrix*/
		void setProjTransform(const Matrix &m);
		/**Sets blend mode. Default value is blend_premultiplied_alpha*/
		void setBlendMode(blend_mode blend);
		/**Sets texture. If texture is null White texture would be used.*/
		void setTexture_(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied = true);
		void setDiffuse(const Diffuse &df);
		void setMask(spNativeTexture mask, const RectF &srcRect, const RectF &destRect, const transform &t);
		void removeMask();
		/**Sets World transformation matrix.*/
		void setTransform(const transform &m);		

		void setShaderProgram(shaderProgram pr);

		/**Returns triangles vertex Color.*/
		const Color &getPrimaryColor() const;
		/**Sets triangles vertex Color.*/
		void setPrimaryColor(const Color &);
		/**Accumulates rectangles into batch or render it.*/
		void draw(const RectF &srcRect, const RectF &destRect);		
		void draw(const void *data, int size, bvertex_format format);
		/**Draws existing batch immediately.*/
		void drawBatch();
		/**Cleans existing accumulated batch.*/
		void cleanup();

		int getMaxVertices() const {return _maxVertices;}		

		IVideoDriver *getDriver();
		void setDriver(IVideoDriver *);


		//debug utils
#ifdef OXYGINE_DEBUG_T2P
		static void showTexel2PixelErrors(bool show);
#endif

	protected:			
		void updateDriver();

		IVideoDriver *_driver;
		Matrix _view;
		Matrix _proj;
		Rect _viewport;
		transform _transform;

		Color _primaryColor;
		Color _primaryColorPremultiplied;
		ClipUV _clipUV;

		spNativeTexture _rt;

		batch _batch;
		unsigned int _maxVertices;		
	};
}