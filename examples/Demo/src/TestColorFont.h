#pragma once
#include "test.h"


class ColoredShaderMat : public STDMaterial
{
public:
    MATX(ColoredShaderMat);

    Vector4 uniformBlack = Vector4(0.0f, 0.0f, 0.0f, 1.0f);


    void rehash(size_t& hash) const override
    {
        STDMaterial::rehash(hash);
        hash_combine(hash, uniformBlack.x, uniformBlack.y, uniformBlack.z, uniformBlack.w);
    }

    static bool cmp(const ColoredShaderMat& a, const ColoredShaderMat& b)
    {
        if (!STDMaterial::cmp(a, b))
            return false;

        return a.uniformBlack == b.uniformBlack;
    }

    void xapply() override
    {
        STDMaterial::xapply();
        IVideoDriver::instance->setUniform("_black", uniformBlack);
    }
};

class ShaderTextField : public TextField
{
public:
    static UberShaderProgram* shader;
    static void init()
    {
        shader = new UberShaderProgram();
        shader->init(STDRenderer::uberShaderBody,
                     R"(
                     #define MODIFY_BASE
                     #define DONT_MULT_BY_RESULT_COLOR)",

                     R"(
                     uniform lowp vec4 _black;
                     uniform lowp vec4 _white;
                     lowp vec4 modify_base(lowp vec4 base)
                     {
                         lowp vec4 black = vec4(_black.rgb, result_color.a);
                         lowp vec4 white = result_color;
                         return mix(white, black, base.r) * base.a;
                     }
            )");
    }

    static void free()
    {
        delete shader;
    }

    ColoredShaderMat mat;

    ShaderTextField() : _outer(Color::White)
    {
        mat._uberShader = shader;
        mat.uniformBlack = _outer.toVector();
        _mat = mc().cache(mat);
    }

    const Color& getOuterColor() const
    {
        return _outer;
    }

    void setOuterColor(const Color& v)
    {
        _outer = v;
        mat.uniformBlack = _outer.toVector();

        setMat(mc().cache(mat));
    }

    typedef Property<Color, const Color&, ShaderTextField, &ShaderTextField::getOuterColor, &ShaderTextField::setOuterColor>   TweenOuterColor;

private:
    Color _outer;
};

DECLARE_SMART(ShaderTextField, spShaderTextField);

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
        txt->setKerning(5);
        txt->addTween(ShaderTextField::TweenOuterColor(Color::Black), 4000, -1, true, 2000);
        txt->addTween(ShaderTextField::TweenColor(Color::Magenta), 5000, -1, true);
    }

    ~TestColorFont()
    {
        ShaderTextField::free();
    }

    void toggleClicked(string id, const toggle* data)
    {

    }
};
