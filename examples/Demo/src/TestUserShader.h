#pragma once
#include "test.h"
#include "core/UberShaderProgram.h"
#include "core/gl/VideoDriverGLES20.h"
#include "core/gl/oxgl.h"
#include "STDMaterial.h"

DECLARE_SMART(ShaderSprite, spShaderSprite);

class ShaderSprite: public Sprite
{
public:
    ShaderSprite(): _program(0), _val(0.5f, 0.5f, 0.5f, 0)
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

    typedef Property<Vector4, const Vector4&, ShaderSprite, &ShaderSprite::getVal, &ShaderSprite::setVal>   TweenVal;

private:
    Vector4 _val;
    UberShaderProgram* _program;
    void setUniforms(IVideoDriver* driver, ShaderProgram* prog)
    {
        driver->setUniform("userValue", &_val, 1);
    }

    void doRender(const RenderState& rs) override
    {
        /* _program->setShaderUniformsCallback(CLOSURE(this, &ShaderSprite::setUniforms));
         STDRenderer* renderer = safeCast<STDRenderer*>(rs.renderer);
         renderer->setUberShaderProgram(_program);
         Sprite::doRender(rs);
         renderer->setUberShaderProgram(&Renderer::uberShader);

         _program->setShaderUniformsCallback(UberShaderProgram::ShaderUniformsCallback());*/
    }
};

class TweenShader: public Tween, public Material
{
public:
    UberShaderProgram* _program;
    Vector4 _val;
    TweenShader(UberShaderProgram* prog) : _program(prog), _val(0, 0, 0, 0) {}
    ~TweenShader()
    {
        if (_client)
            _client->setMaterial(0);
    }

    void _start(Actor& actor) override
    {
        actor.setName("zzz");
        actor.setMaterial(this);
    }

    void _update(Actor& actor, const UpdateState& us) override
    {
        _val = lerp(Vector4(1, 1, 1, 0), Vector4(0, 0, 0, 0),  _percent);
    }

    void _done(Actor& actor, const UpdateState& us) override
    {
        actor.setMaterial(0);
    }

    void setUniforms(IVideoDriver* driver, ShaderProgram* prog)
    {
        driver->setUniform("userValue", &_val, 1);
    }

    void apply(Material* prev) override
    {
        STDRenderer* renderer = STDRenderer::getCurrent();
        _program->setShaderUniformsCallback(CLOSURE(this, &TweenShader::setUniforms));
        renderer->setUberShaderProgram(_program);
    }

    void doRender(Sprite* s, const RenderState& rs) override
    {
        STDMaterial::instance->doRender(s, rs);
    }

    void finish() override
    {
        STDRenderer* renderer = STDRenderer::getCurrent();
        renderer->drawBatch();
    }
};


class TestUserShader: public Test
{
public:
    UberShaderProgram* _shaderMono;
    UberShaderProgram* _shaderAddColor;
    UberShaderProgram* _shaderInvert;

    spSprite _sprite;

    TestUserShader(): _shaderMono(0), _shaderAddColor(0)
    {
        _shaderMono = new UberShaderProgram();
        _shaderMono->init(STDRenderer::uberShaderBody,
                          "#define MODIFY_BASE_PRE\n"
                          "uniform lowp vec4 userValue;"
                          "lowp vec4 modify_base_pre(lowp vec4 base)\n"
                          "{\n"
                          "lowp float c = (base.r + base.g + base.b) / 3.0;\n"
                          "return mix(vec4(c, c, c, base.a), base, userValue.r);\n"
                          "}\n");

        _shaderAddColor = new UberShaderProgram();
        _shaderAddColor->init(STDRenderer::uberShaderBody,
                              "#define MODIFY_BASE_PRE\n"
                              "uniform lowp vec4 userValue;"
                              "lowp vec4 modify_base_pre(lowp vec4 base)\n"
                              "{\n"
                              "return base + userValue * base.a;\n"
                              "}\n");

        _shaderInvert = new UberShaderProgram();
        _shaderInvert->init(STDRenderer::uberShaderBody,
                            "#define MODIFY_BASE_PRE\n"
                            "uniform lowp vec4 userValue;"
                            "lowp vec4 modify_base_pre(lowp vec4 base)\n"
                            "{\n"
                            "\n"
                            "lowp vec4 inverted = vec4(1.0, 1.0, 1.0, 1.0) - base;\n"
                            "lowp vec4 res = mix(inverted * base.a, base, userValue.r);\n"
                            "return vec4(res.rgb, base.a);\n"
                            "}\n");


        _sprite = new Sprite;
        _sprite->setResAnim(resources.getResAnim("bg"));
        _sprite->attachTo(content);

        //_sprite->addTween2(new TweenShader(_shaderInvert), TweenOptions(2000).twoSides(true).loops(-1));

        toggle t[] =
        {
            toggle("->shader:add color", 0, _shaderAddColor),
            toggle("->shader:mono", 0, _shaderMono),
            toggle("->shader:invert", 0, _shaderInvert)
        };
        addToggle("shader", t, 3);
    }

    ~TestUserShader()
    {
        delete _shaderInvert;
        delete _shaderMono;
        delete _shaderAddColor;
    }

    void toggleClicked(string id, const toggle* data)
    {
        if (id == "shader")
        {
            UberShaderProgram* shader = (UberShaderProgram*)data->data;

            _sprite->addTween2(new TweenShader(shader), TweenOptions(3000).twoSides(true));
        }
    }
};
