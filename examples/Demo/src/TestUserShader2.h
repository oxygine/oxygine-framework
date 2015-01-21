#pragma once

#include "test.h"
#include "core/UberShaderProgram.h"
#include "core/gl/VideoDriverGLES20.h"
#include "core/gl/oxgl.h"
#include "core/gl/ShaderProgramGL.h"


class LightningRenderer : public Renderer
{
public:
	spNativeTexture _base;
	spNativeTexture _normal;

	spNativeTexture _base2;

	AnimationFrame _defaultNormal;

	ShaderProgramGL *_lightShader;
	Vector2 _light;
	LightningRenderer() :_light(0, 0)
	{
		//vertex declaration with 2 pairs of UV
		_vdecl = getDriver()->getVertexDeclaration(vertexPCT2T2::FORMAT);

		//load vertex shader
		file::buffer vsdata;
		file::read("light_vs.glsl", vsdata);
		vsdata.push_back(0);///null terminating string
		unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vsdata.front(), "", "");

		//load fragment shader
		file::buffer fsdata;
		file::read("light_fs.glsl", fsdata);
		fsdata.push_back(0);///null terminating string
		unsigned int ps = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fsdata.front(), "", "");

		//link into 1 shader program
		unsigned int pr = ShaderProgramGL::createProgram(vs, ps, static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2T2::FORMAT)));
		_lightShader = new ShaderProgramGL();
		_lightShader->init(pr);

		//set shader and apply samplers uniforms
		getDriver()->setShaderProgram(_lightShader);
		getDriver()->setUniformInt("base_texture", 0);
		getDriver()->setUniformInt("normal_texture", 1);

		//texture for sprites without normals
		_defaultNormal = resources.getResAnim("defnormal")->getFrame(0);
	}

	void setLightPosition(const Vector2 &pos)
	{
		_light = pos;
	}
	
	void _begin()
	{
		setShader(_lightShader);
		_base = 0;
		_normal = 0;

		_driver->setUniform("light", &_light, 1);

		_driver->setState(IVideoDriver::STATE_BLEND, 1);
		_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
		_driver->setBlendFunc(IVideoDriver::BT_SRC_ALPHA, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
	}

	void setBlendMode(blend_mode blend)
	{
	}

	void setTexture(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied /* = true */)
	{
		_base2 = base;
		
	}

	void preDrawBatch()
	{
	}

	void draw(const RState *rs, const Color &color, const RectF &srcRect, const RectF &destRect)
	{
		draw(rs, color, srcRect, destRect, 
			_base2, _defaultNormal.getDiffuse().base, _defaultNormal.getSrcRect());
	}

	void draw(const RState *rs, const Color &color, const RectF &srcRect, const RectF &destRect, spNativeTexture base, spNativeTexture normal, const RectF &normalSrc)
	{	
		if (_base != base)
		{
			drawBatch();
			_base = base;
			_driver->setTexture(0, _base);
		}

		if (_normal != normal)
		{
			drawBatch();
			_normal = normal;
			_driver->setTexture(1, _normal);
		}

		vertexPCT2T2 v[4];		
		fillQuadT2(v, srcRect, normalSrc, destRect, rs->transform, color.rgba());
		_vertices.insert(_vertices.end(), (unsigned char*)v, (unsigned char*)v + sizeof(v));
		_checkDrawBatch();
	}
};

class Sprite2 : public Sprite
{
public:
	Sprite2(const AnimationFrame &n) :normal(n){}

	void doRender(const RenderState &rs)
	{
		LightningRenderer *renderer = safeCast<LightningRenderer *>(rs.renderer);

		const Diffuse &df = _frame.getDiffuse();
		renderer->draw(&rs, getColor(), _frame.getSrcRect(), getDestRect(), df.base, normal.getDiffuse().base, normal.getSrcRect());
	}

	AnimationFrame normal;
};


class LightningActor : public Actor
{
public:
	LightningRenderer _renderer;
	void render(const RenderState &parent)
	{
		_renderer.setLightPosition(_getStage()->getDescendant("light")->getPosition());

		RenderState rs = parent;
		rs.renderer = &_renderer;
		_renderer.begin(parent.renderer);
		Actor::render(rs);
		_renderer.end();
	}
};
class TestUserShader2 : public Test
{
public:
	UberShaderProgram *_shader;

	Draggable drag;

	TestUserShader2() :_shader(0)
	{		
		spActor lightning = new LightningActor;
		this->content->addChild(lightning);
		lightning->setSize(this->content->getSize());

		AnimationFrame frame = resources.getResAnim("normal")->getFrame(0);
		spSprite spr = new Sprite2(frame);
		spr->setResAnim(resources.getResAnim("tiled"));
		spr->setPosition(content->getSize() / 2 - spr->getSize() / 2);
		spr->attachTo(lightning);
		

		spSprite light = new Sprite;
		light->setName("light");
		light->setResAnim(resources.getResAnim("light"));
		light->setAnchor(0.5f, 0.5f);
		drag.init(light.get());
		light->setPosition(getSize() / 2);

		lightning->addChild(light);
	}

};