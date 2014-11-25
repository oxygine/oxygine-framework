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

	class ClipUV
	{
	public:
		ClipUV(){}
		ClipUV(const Vector2 &a, const Vector2 &b, const Vector2 &c, 
			const Vector2 &a_uv, const Vector2 &b_uv, const Vector2 &c_uv):_uvA(a_uv)
		{
			_dAB = b - a;
			_dAC = c - a;

			float v = 1.0f / (_dAB.x * _dAC.y - _dAB.y * _dAC.x);
			_dAB *= v;
			_dAC *= v;

			_dac.x =  _dAC.x * a.y - _dAC.y * a.x;
			_dac.y =-(_dAB.x * a.y - _dAB.y * a.x);

			_dAB *= -1;

			_dAB.x *= -1;
			_dAC.x *= -1;

			_duvAB = b_uv - _uvA;
			_duvAC = c_uv - _uvA;			
		}

		void get(Vector3* vec) const
		{
			vec[0] = Vector3(_dac.x, _dAC.y, _dAC.x);
			vec[1] = Vector3(_dac.y, _dAB.y, _dAB.x);

			vec[2] = Vector3(_uvA.x, _duvAB.x, _duvAC.x);
			vec[3] = Vector3(_uvA.y, _duvAB.y, _duvAC.y);
		}

		Vector2 calc(const Vector2 &pos) const 
		{
			//float a = _dac.x + _dAC.y * pos.x - _dAC.x * pos.y;
			//float b = _dac.y + _dAB.y * pos.x - _dAB.x * pos.y;

			//Vector3 q = 
			
			float a = Vector3(_dac.x, _dAC.y, _dAC.x).dot(Vector3(1, pos.x, pos.y));
			float b = Vector3(_dac.y, _dAB.y, _dAB.x).dot(Vector3(1, pos.x, pos.y));

			//float u = _uvA.x + _duvAB.x * a + _duvAC.x * b;
			//float v = _uvA.y + _duvAB.y * a + _duvAC.y * b;
			float u = Vector3(_uvA.x, _duvAB.x, _duvAC.x).dot(Vector3(1, a, b));
			float v = Vector3(_uvA.y, _duvAB.y, _duvAC.y).dot(Vector3(1, a, b));

			//Matrix

			return Vector2(u, v);
		}

	protected:
		Vector2 _dac;
		Vector2 _uvA;

		Vector2 _dAB;
		Vector2 _dAC;
		Vector2 _duvAB;
		Vector2 _duvAC;
	};
	
	RectF calcUV(const Rect &srcRect, float iw, float ih);

	template<class V>
	void fillQuadT(V *pv, const RectF &srcRect, const RectF &destRect, const AffineTransform &transform, unsigned int rgba)
	{
		float u = srcRect.pos.x;
		float v = srcRect.pos.y;

		float du = srcRect.size.x;
		float dv = srcRect.size.y;	

		V vt;
		vt.color = rgba;

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

	template<class V>
	void fillQuadT2(V *pv, const RectF &srcRect, const RectF &srcRect2, const RectF &destRect, const AffineTransform &transform, unsigned int rgba)
	{
		float u = srcRect.pos.x;
		float v = srcRect.pos.y;

		float du = srcRect.size.x;
		float dv = srcRect.size.y;

		float u2 = srcRect2.pos.x;
		float v2 = srcRect2.pos.y;
		
		float du2 = srcRect2.size.x;
		float dv2 = srcRect2.size.y;

		V vt;
		vt.color = rgba;

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
		vt.u2 = u2;
		vt.v2 = v2;
		*pv = vt;
		++pv;

		vt.x = p2.x;
		vt.y = p2.y;
		vt.u = u;
		vt.v = v + dv;
		vt.u2 = u2;
		vt.v2 = v2 + dv2;
		*pv = vt;
		++pv;

		vt.x = p3.x;
		vt.y = p3.y;
		vt.u = u + du;
		vt.v = v;
		vt.u2 = u2 + du2;
		vt.v2 = v2;
		*pv = vt;
		++pv;

		vt.x = p4.x;
		vt.y = p4.y;
		vt.u = u + du;
		vt.v = v + dv;
		vt.u2 = u2 + du2;
		vt.v2 = v2 + dv2;
		*pv = vt;
		++pv;
	}

	class IVideoDriver;
	class UberShaderProgramBase;
	class UberShaderProgram;
	class Diffuse;


	/*
	class MaterialExt
	{
	public:
		virtual ~MaterialExt(){}
	};

	class Material
	{
	public:
		Material();

		spNativeTexture base;
		spNativeTexture alpha;
		//RectF srcRect;
		bool basePremultiplied;
		blend_mode blend;

		MaterialExt *ext;
	};
	*/

	class RState;

	class Renderer
	{
	public:
		typedef AffineTransform transform;

		static bool getPremultipliedAlphaRender();
		/**There are 2 modes of loading and blending/rendering sprites: normal and premultiplied alpha.
			You should set it before loading any assets. 
			Premultiplied mode is more advanced and faster than normal. In this mode RGB pixels of textures premultiplying to alpha when textures are loading and using blend_premultiply_alpha as default Sprites blend option.
			http://blog.rarepebble.com/111/premultiplied-alpha-in-opengl/
			Default value is premultiplied = true
		*/
		static void setPremultipliedAlphaRender(bool pre);

		/**Sets default rendering opengl options for 2D*/
		static void setDefaultSettings();
		/**Initializes internal classes. Called automatically from oxygine::init();*/
		static void initialize();
		/**Cleans internal static classes*/
		static void release();
		/**for lost context*/
		static void reset();
		static bool isReady();
		/**restore after lost context*/
		static void restore();
		/**White 4x4 Texture*/
		static spNativeTexture white;
		static UberShaderProgram uberShader;
		static std::vector<unsigned char> uberShaderBody;

		static std::vector<unsigned char> indices8;
		static std::vector<unsigned short> indices16;
		static size_t maxVertices;

		Renderer(IVideoDriver *driver = 0);
		virtual ~Renderer();

		const Matrix&	getViewProjection() const {return _vp;}
		IVideoDriver*	getDriver();



		/**Begins rendering into RenderTexture or into primary framebuffer if rt is null*/
		void begin(Renderer *prev);
		/**Completes started rendering and restores previous Framebuffer.*/
		void end();		

		/**initializes View + Projection matrices where TopLeft is (0,0) and RightBottom is (width, height). use flipU = true for render to texture*/
		void initCoordinateSystem(int width, int height, bool flipU = false);
		void setViewProjTransform(const Matrix &view, const Matrix &proj);		

		void setVertexDeclaration(const VertexDeclaration *decl);
		
		/**Sets World transformation matrix.*/
		//void setTransform(const transform &m);				
		void resetSettings();
		void setDriver(IVideoDriver *);
		
		virtual void draw(const RState *r, const Color &, const RectF &srcRect, const RectF &destRect) = 0;
		virtual void setBlendMode(blend_mode bm) = 0;
		virtual void setTexture(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied = true) = 0;

		//virtual void draw(const RState *rs, const void *data, int size, bvertex_format format, bool worldTransform = false) = 0;
		/**Draws existing batch immediately.*/
		void drawBatch();
		/**Cleans existing accumulated batch.*/
		void cleanup();
		

		//debug utils
#ifdef OXYGINE_DEBUG_T2P
		static void showTexel2PixelErrors(bool show);
#endif

		virtual void addVertices(const void *data, unsigned int size);

	protected:			
		Renderer *_previous;
		void setShader(ShaderProgram *prog);

		void _drawBatch();

		virtual void preDrawBatch(){}
		virtual void changedMaterial(){}
		virtual void _cleanup(){}
		virtual void _begin(){}
		virtual void _resetSettings(){}


		
		

		void _addVertices(const void *data, unsigned int size);
		void _checkDrawBatch();

		std::vector<unsigned char> _vertices;
		
		const VertexDeclaration *_vdecl;

		IVideoDriver *_driver;
		ShaderProgram *_program;
		Matrix _vp;
		//transform _transform;
	};
}