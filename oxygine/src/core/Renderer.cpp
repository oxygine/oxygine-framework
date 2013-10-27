#include "Renderer.h"
#include "math/Rect.h"
#include "Actor.h"
#include "MemoryTexture.h"
#include "ImageDataOperations.h"
#include "AnimationFrame.h"
#include "VertexDeclaration.h"

namespace oxygine
{
	Renderer::Stats Renderer::statsPrev;
	Renderer::Stats Renderer::statsCurrent;

	spNativeTexture Renderer::white;

	


	template<class V, class XY>
	void fillQuad(V *v, const RectF &uv, XY *positions, const Color &color)
	{
		const XY &p1 = positions[0];
		const XY &p2 = positions[1];
		const XY &p3 = positions[2];
		const XY &p4 = positions[3];

		V vt;
		vt.color = color.rgba();
		vt.z = 0;

		vt.x = p1.x;
		vt.y = p1.y;				
		vt.u = uv.pos.x;
		vt.v = uv.pos.y;
		*v = vt;
		++v;

		vt.x = p2.x;
		vt.y = p2.y;		
		vt.u = uv.pos.x;
		vt.v = uv.getBottom();
		*v = vt;
		++v;

		vt.x = p3.x;
		vt.y = p3.y;		
		vt.u = uv.getRight();
		vt.v = uv.pos.y;
		*v = vt;
		++v;

		vt.x = p4.x;
		vt.y = p4.y;		
		vt.u = uv.getRight();
		vt.v = uv.getBottom();
		*v = vt;
		++v;
	}
	
	void Renderer::initialize()
	{
		MemoryTexture memwhite;
		memwhite.init(4, 4, TF_R8G8B8A8);

		oxygine::operations::op_fill fill;
		ImageData im = memwhite.lock();
		oxygine::operations::applyOperation(fill, im);

		white = IVideoDriver::instance->createTexture();
		white->setName("!renderer. white");
		white->init(im, false);
		white->setLinearFiltration(false);
	}

	void Renderer::release()
	{
		if (white)
			white->release();
		white = 0;
	}	


	Renderer::Renderer(IVideoDriver *driver):_rt(0), _maxVertices(0), _driver(driver), _viewport(0,0,0,0)
	{
		if (!driver)
			setDriver(IVideoDriver::instance);

		_batch.vertices.reserve(200);
		setPrimaryColor(Color(255,255,255,255));

		_view.identity();
		_transform.identity();
	}
	
	Renderer::~Renderer()
	{
		drawBatch();
	}

	void Renderer::cleanup()
	{
		_batch.base = 0;
		_batch.vertices.resize(0);
	}

	void Renderer::setBlendMode(blend_mode blend)
	{
		batch &b = _batch;
		if (b.blend != blend)
		{
			drawBatch();
		}

		b.blend = blend;
	}

	void Renderer::setDiffuse(const Diffuse &df)
	{
		setTexture_(df.base, df.alpha, df.premultiplied);
	}

	void Renderer::setTexture_(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied)
	{
		if (base == 0 || base->getHandle() == 0)
			base = white;

		batch &b = _batch;
		if (b.base != base || b.alpha != alpha || b.basePremultiplied != basePremultiplied)
		{
			drawBatch();
		}
		b.basePremultiplied = basePremultiplied;
		b.base = base;
		b.alpha = alpha;
	}

	void Renderer::setTransform(const transform &m)
	{
		_transform = m;
	}

	void Renderer::setShaderProgram(shaderProgram pr)
	{
		batch &b = _batch;
		if (b.program != pr)
		{
			drawBatch();
		}

		b.program = pr;
	}

	const Color &Renderer::getPrimaryColor() const
	{
		return _primaryColor;
	}

	void Renderer::setPrimaryColor(const Color &c)
	{
		_primaryColor = c;
		_primaryColorPremultiplied = _primaryColor.premultiplied();
	}

	void Renderer::drawBatch()
	{		
		batch &b = _batch;

		if (!b.vertices.empty())
		{
			getDriver()->drawBatch(b);
			
			++statsCurrent.batches;
			statsCurrent.triangles += b.vertices.size()/(b.vdecl->size * 2);

			b.vertices.resize(0);			
		}
	}

	RectF calcUV(const Rect &srcRect, float iw, float ih)
	{
		float u = srcRect.pos.x * iw;
		float v = srcRect.pos.y * ih;

		float du = srcRect.size.x * iw;
		float dv = srcRect.size.y * ih;	
		
		return RectF (u, v, du, dv);
	}


	bool checkT2P(const Rect &viewport, const Matrix &vp, const vertexPCT2 *v1, const vertexPCT2 *v2, int w, int h)
	{
		Vector3 p1(v1->x, v1->y, 0);
		Vector3 p2(v2->x, v2->y, 0);

		p1 = vp.transformVec3(p1);
		p2 = vp.transformVec3(p2);

		Vector2 half = viewport.getSize().cast<Vector2>() / 2;
		p1.x = p1.x * half.x + half.x;
		p1.y = p1.y * half.y + half.y;

		p2.x = p2.x * half.x + half.x;
		p2.y = p2.y * half.y + half.y;
				
		Vector2 tc1(v1->u, v1->v);
		Vector2 tc2(v2->u, v2->v);
		Vector3 dp_ = p1 - p2;
		Vector2 dp(dp_.x, dp_.y);
		dp.x = scalar::abs(dp.x);
		dp.y = scalar::abs(dp.y);

		Vector2 dtc = tc1 - tc2;
		dtc.x = scalar::abs(dtc.x) * w;
		dtc.y = scalar::abs(dtc.y) * h;

		const float EPS = 0.05f;

		Vector2 d = dp - dtc;
		if (scalar::abs(d.x) >= EPS || scalar::abs(d.y) >= EPS)
			return false;

		p1.x = scalar::abs(p1.x);
		p1.y = scalar::abs(p1.y);

		if (scalar::abs(p1.x - int(p1.x + EPS)) > EPS ||
			scalar::abs(p1.y - int(p1.y + EPS)) > EPS)
			return false;

		return true;
	}
	
	bool _showTexel2PixelErrors = false;

#ifdef OXYGINE_DEBUG_T2P
	void Renderer::showTexel2PixelErrors(bool show)
	{
		_showTexel2PixelErrors = show;
	}
#endif	

	void Renderer::setMask(spNativeTexture mask, const RectF &srcRect, const RectF &destRect, const transform &t, bool channelR)
	{
		batch &b = _batch;
		if (b.mask != mask)
		{
			drawBatch();
		}

		_clipUV = ClipUV(
			t.transform(destRect.getLeftTop()),
			t.transform(destRect.getRightTop()),
			t.transform(destRect.getLeftBottom()),
			srcRect.getLeftTop(),
			srcRect.getRightTop(),
			srcRect.getLeftBottom());

		b.mask = mask;
		b.maskChannelR = channelR;
		if (b.vdecl->bformat != VERTEX_PCT2T2)
			b.vdecl = _driver->getVertexDeclaration(VERTEX_PCT2T2);		
		b.clipMask = srcRect;
		Vector2 v(1.0f / mask->getWidth(), 1.0f / mask->getHeight());
		b.clipMask.expand(v, v);
	}

	void Renderer::removeMask()
	{
		drawBatch();
		batch &b = _batch;
		b.mask = 0;
		b.vdecl = _driver->getVertexDeclaration(VERTEX_PCT2);
	}

	void Renderer::updateDriver()
	{
		if (_batch.vdecl)
			return;
		_batch.vdecl = _driver->getVertexDeclaration(VERTEX_PCT2);
		_maxVertices = _driver->getMaxVertices();
	}

	void Renderer::draw(const RectF &srcRect, const RectF &destRect)
	{
		updateDriver();

		batch &b = _batch; 

		NativeTexture *nt = b.base.get();

		Color color = _primaryColor;
		if (b.blend == blend_premultiplied_alpha)
			color = _primaryColorPremultiplied;

		if (b.mask)
		{
			vertexPCT2T2 v[4];
			fillQuadT(v, srcRect, destRect, _transform, color);		
			
			for (int i = 0; i < 4; ++i)
			{
				Vector2 uv = _clipUV.calc(Vector2(v[i].x, v[i].y));
				v[i].u2 = uv.x;
				v[i].v2 = uv.y;
			}

			b.vertices.insert(b.vertices.end(), (unsigned char*)v, (unsigned char*)v + sizeof(v));

		}
		else
		{
			vertexPCT2 v[4];
			fillQuadT(v, srcRect, destRect, _transform, color);		

#ifdef OXYGINE_DEBUG_T2P
			if (b.base != white && _showTexel2PixelErrors)
			{
				Matrix vp = _view * _proj;

				bool t = checkT2P(_viewport, vp, &v[0], &v[3], nt->getWidth(), nt->getHeight());
				if (!t)
				{
					float c = (sinf((float)getTimeMS()/200 + v[0].x * v[0].y) + 1)/2.0f;
					color = interpolate(Color(rand() % 255, rand() % 255,rand() % 255, 255), color, c);
					fillQuadT(v, srcRect, destRect, _transform, color);		
				}
			}	
#endif

			b.vertices.insert(b.vertices.end(), (unsigned char*)v, (unsigned char*)v + sizeof(v));
		}
		
		if (b.vertices.size()/sizeof(b.vdecl->size) >= _maxVertices)
			drawBatch();
	}

	void Renderer::draw(const void *data, int size, bvertex_format format)
	{
		updateDriver();

		batch &b = _batch; 
		if (b.vdecl->bformat != format)
		{
			drawBatch();
		}
		
		if (b.vdecl->bformat != format)
			b.vdecl = _driver->getVertexDeclaration(format);

		int num = size / b.vdecl->size;
		size_t currentNum = b.vertices.size() / b.vdecl->size;
		if (currentNum + num >= _maxVertices)
		{
			drawBatch();
		}	
		


		b.vertices.insert(b.vertices.end(), (unsigned char*)data, (unsigned char*)data + size);

		
		//if (b.vertices.size()/sizeof(vSize) >= _maxVertices)
		//	drawBatch();
	}


	void Renderer::setDefaultSettings()
	{
		OX_ASSERT(!"NO");
		//_driver->setDefaultSettings();		
	}

	void Renderer::setViewTransform(const Matrix &m)
	{
		_view = m;
	}

	void Renderer::setProjTransform(const Matrix &m)
	{
		_proj = m;
	}

	IVideoDriver *Renderer::getDriver()
	{
		return _driver;
	}

	void Renderer::setDriver(IVideoDriver *driver)
	{
		_driver = driver;
	}

	bool Renderer::begin(spNativeTexture rt, const Rect &viewport, const Color *clearColor)
	{
		if (!getDriver()->isReady())
			return false;
		_rt = rt;

		if (_rt)
		{
			getDriver()->setRenderTarget(_rt);
		}

		_viewport = viewport;

		getDriver()->begin(_proj, _view, viewport, clearColor);

		return true;
	}

	void Renderer::end()
	{
		drawBatch();

		if (_rt)
		{
			getDriver()->setRenderTarget(0);
		}

		_rt = 0;
	}
}