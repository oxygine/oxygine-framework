#pragma once
#include "test.h"
#include "core/UberShaderProgram.h"
#include "core/gl/VideoDriverGLES20.h"
#include "core/gl/oxgl.h"

DECLARE_SMART(ShaderSprite, spShaderSprite);

class ShaderSprite: public Sprite
{
public:
	ShaderSprite():_program(0), _val(0,0,0,0)
	{

	}
	
	void setShaderProgram(UberShaderProgram* p)
	{
		_program = p;
	}

	const Vector4& getVal() const 
	{
		return _val;
	}

	void setVal(const Vector4& v)
	{
		_val = v;
	}

	typedef GetSet<Vector4, const Vector4&, ShaderSprite, &ShaderSprite::getVal, &ShaderSprite::setVal>	TweenVal;

private:
	Vector4 _val;
	UberShaderProgram* _program;
	void setUniforms(ShaderProgram *prog)
	{
		prog->setUniform("userValue", &_val, 1);
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
	UberShaderProgram *_shaderMono;
	UberShaderProgram *_shaderAddColor;
	UberShaderProgram *_shaderInvert;

	spShaderSprite _sprite;

	TestUserShader():_shaderMono(0), _shaderAddColor(0)
	{
		_shaderMono = new UberShaderProgram();
		_shaderMono->init(Renderer::uberShaderBody, 
			"#define MODIFY_BASE\n"
			"uniform lowp vec4 userValue;"
			"lowp vec4 modify_base(lowp vec4 base)\n"
			"{\n"
			"lowp float c = (base.r + base.g + base.b)/3.0;\n"
			"return mix(vec4(c, c, c, base.a), base, userValue.r);\n"
			"}\n");

		_shaderAddColor = new UberShaderProgram();
		_shaderAddColor->init(Renderer::uberShaderBody, 
			"#define MODIFY_BASE\n"
			"uniform lowp vec4 userValue;"
			"lowp vec4 modify_base(lowp vec4 base)\n"
			"{\n"
			"return base + userValue;\n"
			"}\n");

		_shaderInvert = new UberShaderProgram();
		_shaderInvert->init(Renderer::uberShaderBody, 
			"#define MODIFY_BASE\n"
			"uniform lowp vec4 userValue;"
			"lowp vec4 modify_base(lowp vec4 base)\n"
			"{\n"
			"\n"
			"return vec4(mix(vec4(1.0, 1.0, 1.0, 1.0) - base, base, userValue.r).rgb, base.a);\n"
			"}\n");


		_sprite = initActor(new ShaderSprite, 
			arg_resAnim = resources.getResAnim("bg"),
			arg_attachTo = content
			//arg_pos = content->getSize()/2,
			//arg_anchor = Vector2(0.5f, 0.5f)
			);

		_sprite->addTween(ShaderSprite::TweenVal(Vector4(1,1,1,0)), 5000, -1, true);
		_sprite->setShaderProgram(_shaderInvert);
		
		toggle t[] = {
			toggle("->shader:add color", 0, _shaderAddColor),
			toggle("->shader:mono", 0, _shaderMono), 
			toggle("->shader:invert", 0, _shaderInvert)};
		addToggle("shader", t, 3);		
	}

	~TestUserShader()
	{
		delete _shaderInvert;
		delete _shaderMono;
		delete _shaderAddColor;
	}

	void toggleClicked(string id, const toggle *data)
	{
		if (id == "shader")
		{
			UberShaderProgram *shader = (UberShaderProgram *)data->data;
			_sprite->setShaderProgram(shader);
		}
	}
};
