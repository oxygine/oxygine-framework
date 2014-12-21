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
		_vertices.clear();
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

		_vertices.clear();
	}

	void Renderer::drawBatch()
	{
		if (!_vertices.empty())
		{
			preDrawBatch();
			_drawBatch();
		}
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
		_vertices.clear();
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

	Matrix makeViewMatrix(int w, int h, bool flipU)
	{
		//printf("s1\n");
		Matrix view, scale, tr;
		float offset = 0.5f;

		offset = 0;

		Matrix::translation(tr, Vector3(-(float)w / 2.0f - offset, (flipU ? -1.0f : 1.0f) * (float)h / 2.0f + offset, 0.0f));
		Matrix::scaling(scale, Vector3(1.0f, flipU ? 1.0f : -1.0f, 1.0f));

		view = scale * tr;

		return view;
	}	
}