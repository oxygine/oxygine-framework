#pragma once
#include "test.h"

template <class T>
class IsSame
{
public:
    static bool isSame(const T& a, const T &b)
    {
        return a.isSame(b);
    }
};
template <class T>
bool isSameF(const T &a, const T & b)
{
    return a.isSame(b);
}

class MyTestMat : public MaterialTX<STDMatData>
{
public:
    Vector4 uniform;

    //MyTestMat():MaterialTX<STDMatData>( {}
    MyTestMat() {}
    MyTestMat(const MyTestMat &other)
    {
        _data = other._data;
        _data.init(_hash);

        typedef bool(*fcmp)(const MyTestMat& a, const MyTestMat& b);
        fcmp fn = &IsSame<MyTestMat>::isSame;
        _compare = (compare)(fn);

        hash_combine(_hash, _compare);

  //      hash_combine(hash, uniform.x, uniform.y, uniform.z, uniform.w);
    }

    bool isSame(const MyTestMat& other) const
    {
        if (MaterialTX<STDMatData>::cmp(*this, other))
            return false;

        return uniform == other.uniform;
    }

    MyTestMat* clone() const override
    {
        return new MyTestMat(*this);
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
        actor.setName("zzz");
        Sprite& spr = (Sprite&)actor;

        MyTestMat my;
        my._data = spr._mat->_data;

        spr._mat = mc().add2(my);
//        MyTestMat data;
        //data = spr._mat->_data;
        //spr._mat = mc().add(data);
        //actor.setMaterial(this);
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
        driver->setUniform("userValue", _val);
    }

    /*
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
    */
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
};
