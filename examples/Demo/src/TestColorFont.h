#pragma once
#include "test.h"
#include "core/UberShaderProgram.h"

DECLARE_SMART(ShaderTextField, spShaderTextField);

class ShaderTextField : public TextField
{
public:
    static UberShaderProgram* shader;
    static void init()
    {
        shader = new UberShaderProgram();
        shader->init(Renderer::uberShaderBody,
                     "#define MODIFY_BASE\n"
                     "#define DONT_MULT_BY_RESULT_COLOR\n"
                     "uniform lowp vec4 _black;"
                     "uniform lowp vec4 _white;"
                     "lowp vec4 modify_base(lowp vec4 base)\n"
                     "{\n"
                     "lowp vec4 black = vec4(_black.rgb, 1.0);" "\n"
                     "lowp vec4 white = vec4(_white.rgb, 1.0);" "\n"
                     "return mix(_white, _black, base.r) * base.a;\n"
                     "}\n");
    }

    static void free()
    {
        delete shader;
    }

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

    Vector4 tovec(const Color& c)
    {
        return Vector4(c.getRedF(), c.getGreenF(), c.getBlueF(), c.getAlphaF());
    }

    void setUniforms(IVideoDriver* driver, ShaderProgram* prog)
    {
        Color q(_adata, _adata, _adata, _adata);
        Vector4 c;
        c = tovec(_outer * q);
        driver->setUniform("_black", &c, 1);
        c = tovec(getColor() * _style.color * q);
        driver->setUniform("_white", &c, 1);
    }

    void doRender(const RenderState& rs)
    {
        _adata = rs.alpha;
        shader->setShaderUniformsCallback(CLOSURE(this, &ShaderTextField::setUniforms));
        STDRenderer* renderer = safeCast<STDRenderer*>(rs.renderer);
        renderer->setUberShaderProgram(shader);
        TextField::doRender(rs);
        renderer->setUberShaderProgram(&Renderer::uberShader);

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
        st.font = resources.getResFont("num_fnt_shdr")->getFont();
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
