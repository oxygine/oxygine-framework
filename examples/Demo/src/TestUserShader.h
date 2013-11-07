#pragma once
#include "test.h"
#include "core/UberShaderProgram.h"
#include "core/gl/VideoDriverGLES20.h"
#include "core/gl/oxgl.h"

DECLARE_SMART(ShaderSprite, spShaderSprite);

class ShaderSprite: public Sprite
{
public:
	ShaderSprite():_program(0), _val(0)
	{

	}
	
	void setShaderProgram(UberShaderProgram* p)
	{
		_program = p;
	}

	float getVal() const 
	{
		return _val;
	}

	void setVal(float v)
	{
		_val = v;
	}

	typedef GetSet<float, float, ShaderSprite, &ShaderSprite::getVal, &ShaderSprite::setVal>	TweenVal;

private:
	float _val;
	UberShaderProgram* _program;
	void setUniforms(ShaderProgram *prog)
	{
		prog->setUniform("interp", _val);
	}

	void doRender(const RenderState &rs)
	{
		_program->setShaderUniformsCallback(CLOSURE(this, &ShaderSprite::setUniforms));
		rs.renderer->setUberShaderProgram(_program);
		Sprite::doRender(rs);
		rs.renderer->setUberShaderProgram(&Renderer::uberShader);

		_program->setShaderUniformsCallback(UberShaderProgram::ShaderUniformsCallback());
	}
};

class TestUserShader: public Test
{
public:
	UberShaderProgram *_shader;
	TestUserShader():_shader(0)
	{
		addButton("test", "test");

		_shader = new UberShaderProgram();
		_shader->init(Renderer::uberShaderBody, 
			"#define MODIFY_BASE\n"
			"uniform lowp float interp;"
			"lowp vec4 modify_base(lowp vec4 base)\n"
			"{\n"
			"lowp float c = (base.r + base.g + base.b)/3.0;\n"
			"return mix(vec4(c, c, c, base.a), base, interp);\n"
			"}\n");
	}

	~TestUserShader()
	{
		delete _shader;
	}

	void clicked(string id)
	{
		Vector2 size = content->getSize();
		spNativeTexture texture;
		texture = IVideoDriver::instance->createTexture();
		texture->init((int)size.x, (int)size.y, TF_R8G8B8A8, true);

		Renderer r;
		RenderState rs;
		rs.renderer = &r;


		//Point size = content->getSize().cast<Point>();
		Matrix view = makeViewMatrix((int)size.x, (int)size.y, true);
		Matrix proj;
		Matrix::orthoLH(proj, size.x, size.y, 0.0f, 1.0f);
		r.setViewProjTransform(view, proj);

		Rect vp(Point(0, 0), size.cast<Point>());

		r.begin(texture, vp, 0);
		getRoot()->render(r);
		r.end();


		spShaderSprite sprite = new ShaderSprite;			
		Vector2 pos(50, 50);
		sprite->setPosition(pos);

		AnimationFrame frame;
		Diffuse df;
		df.base = texture;
		frame.init(0, df,
			RectF(0, 0, size.x/texture->getWidth(), size.y/texture->getHeight()), 
			RectF(Vector2(0,0), size), size);
		sprite->setAnimFrame(frame);		


		sprite->attachTo(content);
		sprite->setShaderProgram(_shader);
		sprite->addTween(ShaderSprite::TweenVal(1), 5000, -1, true);
	}
};
