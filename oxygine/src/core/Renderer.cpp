#include "Renderer.h"
#include "math/Rect.h"
#include "Actor.h"
#include "MemoryTexture.h"
#include "ImageDataOperations.h"
#include "AnimationFrame.h"
#include "VertexDeclaration.h"
#include "gl/VideoDriverGLES20.h"

#include "ZipFileSystem.h"
#include "system_data.h"
#include "UberShaderProgram.h"

namespace oxygine
{
	bool _premultipliedRender = true;
	bool Renderer::getPremultipliedAlphaRender()
	{
		return _premultipliedRender;
	}

	void Renderer::setPremultipliedAlphaRender(bool pre)
	{
		_premultipliedRender = pre;
	}


	//Renderer::Stats Renderer::statsPrev;
	//Renderer::Stats Renderer::statsCurrent;
	bool _restored = false;
	spNativeTexture Renderer::white;
	std::vector<unsigned char> Renderer::indices8;
	std::vector<unsigned short> Renderer::indices16;
	size_t _maxVertices = 0;
	UberShaderProgram Renderer::uberShader;
	std::vector<unsigned char> Renderer::uberShaderBody;
	

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
		indices8.reserve(60 * 4);
		for (int t = 0; t < 60; t += 1)
		{
			int i = t * 4;
			indices8.push_back(i + 0);
			indices8.push_back(i + 1);
			indices8.push_back(i + 2);

			indices8.push_back(i + 2);
			indices8.push_back(i + 1);
			indices8.push_back(i + 3);
		}

		indices16.reserve(12000 * 6);
		for (int t = 0; t < 12000; t += 1)
		{
			int i = t * 4;
			indices16.push_back(i + 0);
			indices16.push_back(i + 1);
			indices16.push_back(i + 2);

			indices16.push_back(i + 2);
			indices16.push_back(i + 1);
			indices16.push_back(i + 3);
		}

		_maxVertices = indices16.size()/3 * 2;

		file::buffer shaderBody;
		file::read("shader.glsl", shaderBody, ep_ignore_error);
        uberShaderBody = shaderBody.data;
		if (!shaderBody.getSize())
		{
			file::Zips zp;
			zp.add(system_data, system_size);
			zp.read("system/shader.glsl", shaderBody);
			uberShaderBody = shaderBody.data;
		}

		uberShader.init(uberShaderBody);

		restore();
	}

	void Renderer::release()
	{
		indices8.clear();
		indices16.clear();
		uberShader.release();
		uberShaderBody.clear();
		if (white)
			white->release();
		white = 0;
	}	

	void Renderer::reset()
	{
		_restored = false;
		if (white)
			white->release();
		white = 0;
		uberShader.release();
	}

	void Renderer::restore()
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

		setDefaultSettings();
		_restored = true;
	}


	Renderer::Renderer(IVideoDriver *driver):_rt(0), _driver(driver), 
		_viewport(0,0,0,0), _program(0), _uberShader(0), _blend(blend_disabled), 
		_shaderFlags(0), _clipMask(0,0,0,0), _vdecl(0)
	{
		if (!driver)
			driver = IVideoDriver::instance;

		if (driver)
			setDriver(driver);

		_vertices.reserve(32 * 1000);
		setPrimaryColor(Color(0xffffffff));

		//_view.identity();
		//_proj.identity();
		_transform.identity();
		_vp.identity();

		_uberShader = &uberShader;
	}
	
	Renderer::~Renderer()
	{
		drawBatch();
	}

	void Renderer::cleanup()
	{
		_base = 0;
		_mask = 0;
		_alpha =0;
		_vertices.resize(0);
	}

	void Renderer::setBlendMode(blend_mode blend)
	{
		if (_blend != blend)
		{
			drawBatch();

			switch (blend)
			{
			case blend_disabled:
				_driver->setState(IVideoDriver::STATE_BLEND, 0);
				break;
			case blend_premultiplied_alpha:
				_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_alpha:
				_driver->setBlendFunc(IVideoDriver::BT_SRC_ALPHA, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_add:
				_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
				break;
			//case blend_sub:
				//_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
				//glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			//	break;
			default:
				OX_ASSERT(!"unknown blend");
			}

			if (_blend == blend_disabled)
			{
				_driver->setState(IVideoDriver::STATE_BLEND, 1);
			}
			_blend = blend;
		}
	}

	void Renderer::setDiffuse(const Diffuse &df)
	{
		setTexture_(df.base, df.alpha, df.premultiplied);
	}

	void Renderer::setTexture_(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied)
	{
		if (base == 0 || base->getHandle() == 0)
			base = white;

		unsigned int shaderFlags = _shaderFlags;

		if (basePremultiplied)			
			shaderFlags &= ~UberShaderProgram::ALPHA_PREMULTIPLY;
		else
			shaderFlags |= UberShaderProgram::ALPHA_PREMULTIPLY;
		
		if (alpha)
			shaderFlags |= UberShaderProgram::SEPARATE_ALPHA;
		else
			shaderFlags &= ~UberShaderProgram::SEPARATE_ALPHA;
				
//##ifdef OX_DEBUG
#if 0
		if (_base != base){OX_ASSERT(_alpha != alpha);}
		else{OX_ASSERT(_alpha == alpha);}
#endif //OX_DEBUG

		//no reason to check changed alpha because it is in pair with base
		if (_base != base || /*_alpha != alpha || */_shaderFlags != shaderFlags)
		{			
			drawBatch();
		}

		_shaderFlags = shaderFlags;
		
		_base = base;
		_alpha = alpha;
	}

	void Renderer::setTransform(const transform &m)
	{
		_transform = m;
	}

	void Renderer::setShader(ShaderProgram *prog)
	{
		if (prog != _program)
		{
			_driver->setShaderProgram(prog);
			_driver->setUniform("mat", &_vp);
		}
		_program = prog;
	}

	void Renderer::setUberShaderProgram(UberShaderProgram* pr)
	{
		if (_uberShader != pr)
		{
			drawBatch();
		}

		_uberShader = pr;
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
		if (!_vertices.empty())
		{
			ShaderProgram *prog = _uberShader->getShaderProgram(_shaderFlags)->program;
			setShader(prog);

			_driver->setTexture(UberShaderProgram::SAMPLER_BASE, _base);

			if (_alpha)
			{
				_driver->setTexture(UberShaderProgram::SAMPLER_ALPHA, _alpha);
			}

			if (_mask) 
			{
				_driver->setTexture(UberShaderProgram::SAMPLER_MASK, _mask);

				Vector4 v(_clipMask.getLeft(), _clipMask.getTop(), _clipMask.getRight(), _clipMask.getBottom());
				_driver->setUniform("clip_mask", &v, 1);
			}

			
			UberShaderProgram::ShaderUniformsCallback cb = _uberShader->getShaderUniformsCallback();
			if (cb)
			{
				cb(_driver, prog);
			}			

			size_t count = _vertices.size() / _vdecl->size;
			size_t indices = (count * 3)/2;

			if (indices <= indices8.size())
				getDriver()->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_vertices.front(), count, &indices8.front(), indices, false);
			else
				getDriver()->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_vertices.front(), count, &indices16.front(), indices, true);
		
			_vertices.resize(0);			
			//b.shaderFlags = 0;
		}
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
		if (_mask != mask)
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

		_mask = mask;
		if (_vdecl->bformat != VERTEX_PCT2T2)
			_vdecl = _driver->getVertexDeclaration(VERTEX_PCT2T2);		
		_clipMask = srcRect;
		Vector2 v(1.0f / mask->getWidth(), 1.0f / mask->getHeight());
		_clipMask.expand(v, v);

		_shaderFlags |= UberShaderProgram::MASK;
		if (channelR)
			_shaderFlags |= UberShaderProgram::MASK_R_CHANNEL;
	}

	void Renderer::removeMask()
	{
		drawBatch();

		_mask = 0;
		_shaderFlags &= ~(UberShaderProgram::MASK | UberShaderProgram::MASK_R_CHANNEL);
		_vdecl = _driver->getVertexDeclaration(VERTEX_PCT2);
	}

	Vector2	Renderer::getMaskedUV(const Vector2 &pos)
	{
		Vector2 uv = _clipUV.calc(pos);
		return uv;
	}

	void Renderer::draw(const RectF &srcRect, const RectF &destRect)
	{
		const Color* color = &_primaryColor;
		if (_blend == blend_premultiplied_alpha)
			color = &_primaryColorPremultiplied;

		if (_mask)
		{
			vertexPCT2T2 v[4];
			fillQuadT(v, srcRect, destRect, _transform, *color);
			
			for (int i = 0; i < 4; ++i)
			{
				Vector2 uv = _clipUV.calc(Vector2(v[i].x, v[i].y));
				v[i].u2 = uv.x;
				v[i].v2 = uv.y;
			}

			_vertices.insert(_vertices.end(), (unsigned char*)v, (unsigned char*)v + sizeof(v));
		}
		else
		{
			vertexPCT2 v[4];
			fillQuadT(v, srcRect, destRect, _transform, *color);		

#ifdef OXYGINE_DEBUG_T2P
			if (_base != white && _showTexel2PixelErrors)
			{
				bool t = checkT2P(_viewport, _vp, &v[0], &v[3], _base->getWidth(), _base->getHeight());
				if (!t)
				{
					float c = (sinf((float)getTimeMS()/200 + v[0].x * v[0].y) + 1)/2.0f;
					Color b = interpolate(Color(rand() % 255, rand() % 255,rand() % 255, 255), *color, c);
					fillQuadT(v, srcRect, destRect, _transform, b);		
				}
			}	
#endif

			_vertices.insert(_vertices.end(), (unsigned char*)v, (unsigned char*)v + sizeof(v));
		}
		
		if (_vertices.size()/sizeof(_vdecl->size) >= _maxVertices)
			drawBatch();
	}

	void Renderer::draw(const void *data, int size, bvertex_format format)
	{
		if (_vdecl->bformat != format)
		{
			drawBatch();
			_vdecl = _driver->getVertexDeclaration(format);
		}

		int num = size / _vdecl->size;
		size_t currentNum = _vertices.size() / _vdecl->size;
		if (currentNum + num >= _maxVertices)
		{
			drawBatch();
		}		

		_vertices.insert(_vertices.end(), (unsigned char*)data, (unsigned char*)data + size);
	}

	


	void Renderer::setDefaultSettings()
	{
		/*
		_blend = blend_disabled;
		IVideoDriver* instance = IVideoDriver::instance;
		instance->setState(IVideoDriver::STATE_BLEND, 0);
		*/
		/*
		
		instance->setState(IVideoDriver::STATE_BLEND, 1);
		instance->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
		*/
	}

	void Renderer::initCoordinateSystem(int width, int height, bool flipU)
	{
		Matrix view = makeViewMatrix(width, height, flipU); 
		Matrix proj;
		//initialize projection matrix
		Matrix::orthoLH(proj, (float)width, (float)height, 0, 1);

		setViewProjTransform(view, proj);
	}

	void Renderer::setViewProjTransform(const Matrix &view, const Matrix &proj)
	{
		_vp = view * proj;
	}

	IVideoDriver *Renderer::getDriver()
	{
		return _driver;
	}

	void Renderer::setDriver(IVideoDriver *driver)
	{
		_driver = driver;
	}

	void Renderer::resetSettings()
	{
		_blend = blend_disabled;
		_driver->setState(IVideoDriver::STATE_BLEND, 0);
		_uberShader = &uberShader;
		_program = 0;
	}

	bool Renderer::begin(spNativeTexture rt, const Rect &viewport, const Color *clearColor)
	{
		if (!_restored)
			return false;

		//if (!getDriver()->isReady())
		//	return false;
		_rt = rt;

		if (_rt)
		{
			getDriver()->setRenderTarget(_rt);
		}

		_viewport = viewport;
		_program = 0;

		getDriver()->begin(viewport, clearColor);
		_vdecl = _driver->getVertexDeclaration(VERTEX_PCT2);
		_base = 0;
		_mask = 0;
		_alpha = 0;
		_program = 0;
		_vertices.resize(0);
		resetSettings();		

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