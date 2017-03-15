#pragma once
#include "test.h"


DECLARE_SMART(ShaderTextField, spShaderTextField);
class CustomShader : public Material
{
public:
    static CustomShader* get(Actor* a)
    {
        return safeCast<CustomShader*>(a->getMaterial());
    }

    CustomShader(UberShaderProgram* shader): _shader(shader) {}

    void apply(Actor* a)
    {
        a->setMaterial(this);
    }

    void free()
    {

    }

    void setUniforms(IVideoDriver* driver, ShaderProgram* prog)
    {
        //driver->setUniform("userValue", &_val, 1);

        /*Color q(_adata, _adata, _adata, _adata);
        Vector4 c;
        c = tovec(_outer * q);
        driver->setUniform("_black", &c, 1);
        c = tovec(getColor() * _style.color * q);
        driver->setUniform("_white", &c, 1);*/
    }

    void doRender(Actor* actor, RenderState& rs)
    {
        /* _shader->setShaderUniformsCallback(CLOSURE(this, &CustomShader::setUniforms));
         STDRenderer* renderer = safeCast<STDRenderer*>(rs.renderer);
         renderer->setUberShaderProgram(_shader);

         actor->doRender(rs);

         renderer->setUberShaderProgram(&Renderer::uberShader);
         _shader->setShaderUniformsCallback(UberShaderProgram::ShaderUniformsCallback());*/
    }

    UberShaderProgram* _shader;
};

class ShaderTextField : public TextField
{
public:
    static UberShaderProgram* shader;
    static void init()
    {
        shader = new UberShaderProgram();
        shader->init(STDRenderer::uberShaderBody, R"(
                     #define MODIFY_BASE
                     #define DONT_MULT_BY_RESULT_COLOR
                     uniform lowp vec4 _black;
                     uniform lowp vec4 _white;
                     lowp vec4 modify_base(lowp vec4 base)
                     {
                         lowp vec4 black = vec4(_black.rgb, 1.0);
                         lowp vec4 white = vec4(_white.rgb, 1.0);
                         return mix(_white, _black, base.r) * base.a;
                     }
        )");
    }

    static void free()
    {
        delete shader;
    }

    //void set

    ShaderTextField() : _outer(Color::White)
    {

    }

    const Color& getOuterColor() const
    {
        return _outer;
    }

    void setOuterColor(const Color& v)
    {
        _outer = v;
    }

    typedef Property<Color, const Color&, ShaderTextField, &ShaderTextField::getOuterColor, &ShaderTextField::setOuterColor>   TweenOuterColor;

private:
    Color _outer;
    unsigned char _adata;

    void setUniforms(IVideoDriver* driver, ShaderProgram* prog)
    {
        Color q(_adata, _adata, _adata, _adata);
        Vector4 c = (_outer * q).toVector();
        driver->setUniform("_black", c);
        c = (getColor() * _style.color * q).toVector();
        driver->setUniform("_white", c);
    }

    void doRender(const RenderState& rs)
    {
        Material::setCurrent(rs.material);

        STDRenderer* renderer = STDRenderer::getCurrent();
        _adata = rs.alpha;
        shader->setShaderUniformsCallback(CLOSURE(this, &ShaderTextField::setUniforms));
        renderer->setUberShaderProgram(shader);
        TextField::doRender(rs);
        renderer->setUberShaderProgram(&STDRenderer::uberShader);

        shader->setShaderUniformsCallback(UberShaderProgram::ShaderUniformsCallback());
    }
};

UberShaderProgram* ShaderTextField::shader = 0;


class TestColorFont : public Test
{
public:
    spTextField _txt;

    TestColorFont()
    {
        ShaderTextField::init();

        spShaderTextField txt = new ShaderTextField;
        //spTextField txt = new TextField;
        txt->attachTo(content);
        _txt = txt;

        TextStyle st;
        st.font = resources.getResFont("num_fnt_shdr");
        st.vAlign = TextStyle::VALIGN_MIDDLE;
        st.hAlign = TextStyle::HALIGN_MIDDLE;
        //st.color = Color::CornflowerBlue;
        st.multiline = true;
        txt->setStyle(st);
        txt->setColor(Color::CornflowerBlue);
        txt->setText("1234567890");
        txt->setPosition(getStage()->getSize() / 2);
        txt->setOuterColor(Color::White);
        txt->addTween(ShaderTextField::TweenOuterColor(Color::Black), 4000, -1, true, 2000);
        txt->addTween(TextField::TweenColor(Color::Magenta), 5000, -1, true);
    }

    ~TestColorFont()
    {
        ShaderTextField::free();
    }

    void toggleClicked(string id, const toggle* data)
    {

    }
};
