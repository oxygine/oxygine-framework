#pragma once
#include "test.h"


class MyTestMat : public MaterialTX<STDMatData>
{
public:
    MATX(MyTestMat);

    Vector4 uniform = Vector4(0.0f, 0.0f, 0.0f, 0.0f);


    void rehash(size_t& hash) const override
    {
        data.init(hash);
        hash_combine(hash, uniform.x, uniform.y, uniform.z, uniform.w);
    }

    static bool cmp(const MyTestMat& a, const MyTestMat& b)
    {
        if (!MaterialTX<STDMatData>::cmp(a, b))
            return false;

        return a.uniform == b.uniform;
    }

    void apply()
    {
        MaterialTX<STDMatData>::apply();
        IVideoDriver::instance->setUniform("userValue", uniform);
    }
};


class TweenShader: public Tween//, public Material
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
        Sprite& spr = (Sprite&)actor;

        MyTestMat my;
        my.data = spr._mat->data;
        my.data.us = _program;
        my.uniform = _val;

        spr._mat = mc().add2(my);
    }

    void _update(Actor& actor, const UpdateState& us) override
    {
        Sprite& spr = (Sprite&)actor;

        _val = lerp(Vector4(1, 1, 1, 0), Vector4(0, 0, 0, 0),  _percent);

        MyTestMat my;
        my.data = spr._mat->data;
        my.data.us = _program;
        my.uniform = _val;


        spr._mat = mc().add2(my);
    }

    void _done(Actor& actor, const UpdateState& us) override
    {
        Sprite& spr = (Sprite&)actor;
        STDMaterialX mat;
        mat.data = spr._mat->data;
        spr._mat = mc().add2(mat);
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
        _shaderMono->init(STDRenderer::uberShaderBody, R"(
                          #define MODIFY_BASE_PRE
                          uniform lowp vec4 userValue;
                          lowp vec4 modify_base_pre(lowp vec4 base)
                          {
                              lowp float c = (base.r + base.g + base.b) / 3.0;
                              return mix(vec4(c, c, c, base.a), base, userValue.r);
                          }
        )");

        _shaderAddColor = new UberShaderProgram();
        _shaderAddColor->init(STDRenderer::uberShaderBody, R"(
                              #define MODIFY_BASE_PRE
                              uniform lowp vec4 userValue;
                              lowp vec4 modify_base_pre(lowp vec4 base)
                              {
                                return base + userValue * base.a;
                              }
        )");

        _shaderInvert = new UberShaderProgram();
        _shaderInvert->init(STDRenderer::uberShaderBody, R"(
                            #define MODIFY_BASE_PRE
                            uniform lowp vec4 userValue;
                            lowp vec4 modify_base_pre(lowp vec4 base)
                            {                            
                                lowp vec4 inverted = vec4(1.0, 1.0, 1.0, 1.0) - base;
                                lowp vec4 res = mix(inverted * base.a, base, userValue.r);
                                return vec4(res.rgb, base.a);
                            }
        )");


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

        addButton("blend", "blend");
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

            _sprite->removeTweens();
            _sprite->addTween2(new TweenShader(shader), TweenOptions(3000).twoSides(true).loops(-1));
        }
    }

    void clicked(string id)
    {
        if (id == "blend")
        {
            _sprite->setBlendMode(blend_disabled);
        }
    }
};
