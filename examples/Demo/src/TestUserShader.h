#pragma once
#include "test.h"


class CustomUniformMat : public MaterialTX<STDMatData>
{
public:
    MATX(CustomUniformMat);

    Vector4 uniform = Vector4(0.0f, 0.0f, 0.0f, 0.0f);


    void rehash(size_t& hash) const override
    {
        data.init(hash);
        hash_combine(hash, uniform.x, uniform.y, uniform.z, uniform.w);
    }

    static bool cmp(const CustomUniformMat& a, const CustomUniformMat& b)
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


class TweenUniform: public Tween
{
public:
    TweenUniform() {}

    void _start(Actor& actor) override
    {
        upd(actor, Vector4(0, 0, 0, 0));
    }

    void upd(Actor& actor, const Vector4& val)
    {
        Sprite& spr = (Sprite&)actor;

        CustomUniformMat& my = *safeCast<CustomUniformMat*>(spr._mat.get());
        my.uniform = val;

        spr._mat = mc().cache(my);
    }

    void _update(Actor& actor, const UpdateState& us) override
    {
        Vector4 val = lerp(Vector4(1, 1, 1, 0), Vector4(0, 0, 0, 0),  _percent);
        upd(actor, val);
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
            _sprite->removeTweens();

            UberShaderProgram* shader = (UberShaderProgram*)data->data;

            CustomUniformMat mat;
            mat.data = _sprite->_mat->data;
            mat.data._uberShader = shader;
            _sprite->_mat = mc().cache(mat);

            _sprite->addTween2(new TweenUniform(), TweenOptions(3000).twoSides(true).loops(-1));
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
