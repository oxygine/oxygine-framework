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

#include "RenderState.h"

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
	size_t Renderer::maxVertices = 0;
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

		maxVertices = indices16.size()/3 * 2;

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

	bool Renderer::isReady()
	{
		return _restored;
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


	Renderer::Renderer(IVideoDriver *driver) :_driver(driver),
		_program(0), 
		 _vdecl(0), _previous(0)
	{
		if (!driver)
			driver = IVideoDriver::instance;

		if (driver)
			setDriver(driver);

		_vertices.reserve(32 * 1000);

		_vp.identity();		

		_vdecl = _driver->getVertexDeclaration(vertexPCT2::FORMAT);
	}
	
	Renderer::~Renderer()
	{
		drawBatch();
	}

	void Renderer::cleanup()
	{
		_cleanup();
		_vertices.resize(0);
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
	

	void Renderer::_drawBatch()
	{
		size_t count = _vertices.size() / _vdecl->size;
		size_t indices = (count * 3) / 2;

		if (indices <= indices8.size())
			getDriver()->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_vertices.front(), count, &indices8.front(), indices, false);
		else
			getDriver()->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_vertices.front(), count, &indices16.front(), indices, true);

		_vertices.resize(0);
	}

	void Renderer::drawBatch()
	{
		if (!_vertices.empty())
		{
			preDrawBatch();
			_drawBatch();
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

		

	/*
	void Renderer::draw(const void *data, int size, bvertex_format format, bool worldTransform)
	{
		if (_vdecl->bformat != format)
		{
			drawBatch();
			_vdecl = _driver->getVertexDeclaration(format);
		}

		int num = size / _vdecl->size;
		size_t currentNum = _vertices.size() / _vdecl->size;
		if (currentNum + num >= maxVertices)
		{
			drawBatch();
		}		

		if (worldTransform)
		{
			const unsigned char *ptr = (const unsigned char *)data;
			for (int i = 0; i < num; ++i)
			{
				const Vector2 *pos = (Vector2 *)ptr;
				Vector2 t = _transform.transform(*pos);

				append(_vertices, t);
				_vertices.insert(_vertices.end(), ptr + sizeof(t), ptr + sizeof(t) + _vdecl->size - sizeof(t));

				ptr += _vdecl->size;
			}
			
		}
		else
			_vertices.insert(_vertices.end(), (unsigned char*)data, (unsigned char*)data + size);
			
	}
	*/

	


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
		_resetSettings();
		_driver->setState(IVideoDriver::STATE_BLEND, 0);
		_program = 0;
	}
	
	void Renderer::begin(Renderer *prev)
	{
		OX_ASSERT(_vertices.empty() == true);
		_previous = prev;
		if (_previous)
		{
			_previous->end();
			_vp = _previous->_vp;
		}

		_program = 0;
		_vertices.resize(0);
		resetSettings();		

		_begin();		
	}

	void Renderer::end()
	{
		drawBatch();

		if (_previous)
			_previous->begin(0);
	}

	void Renderer::setVertexDeclaration(const VertexDeclaration *decl)
	{
		if (_vdecl != decl)
			drawBatch();
		_vdecl = decl;
	}

	void Renderer::addVertices(const void *data, unsigned int size)
	{
		_addVertices(data, size);
		_checkDrawBatch();
	}

	void Renderer::_addVertices(const void *data, unsigned int size)
	{
		_vertices.insert(_vertices.end(), (const unsigned char*)data, (const unsigned char*)data + size);
	}

	void Renderer::_checkDrawBatch()
	{
		if (_vertices.size() / sizeof(_vdecl->size) >= maxVertices)
			drawBatch();
	}
}