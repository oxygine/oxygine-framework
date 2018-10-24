#pragma once
#include "test.h"

DECLARE_SMART(SDFMaterial, spSDFMaterial);
class SDFMaterial : public STDMaterial
{
public:
    MATX(SDFMaterial);


    Color outlineColor = Color(255, 255, 255, 255);
    float offset = 0.5f;
    float outline = 0.4f;

    static UberShaderProgram* uberShader;


    static void initMaterial()
    {
        uberShader = new UberShaderProgram();
        uberShader->init(STDRenderer::uberShaderBody,
                         R"(
                #define REPLACED_GET_BASE
                #define DONT_MULT_BY_RESULT_COLOR

                uniform lowp vec4 sdf_outline_color;
                uniform mediump vec4 sdf_params;
            )",

                         R"(
#ifdef PS
                lowp vec4 replaced_get_base()
                {
                    lowp float tx = texture2D(base_texture, result_uv).r;
     
                    
                    lowp float b =   min((tx - sdf_params.z) * sdf_params.w, 1.0);
                    lowp float a = clamp((tx - sdf_params.x) * sdf_params.y, 0.0, 1.0);
	                lowp vec4 res = (sdf_outline_color + (result_color - sdf_outline_color)*a) * b;

                    return res;
                }
#endif
        )");
    }

    static void freeMaterial()
    {
        delete uberShader;
    }

    void init() override
    {
        _uberShader = uberShader;
    }


    void rehash(size_t& hash) const override
    {
        hash_combine(hash, this);
    }

    static bool cmp(const SDFMaterial& a, const SDFMaterial& b)
    {
        if (!STDMaterial::cmp(a, b))
            return false;

        return a.outlineColor == b.outlineColor && a.offset == b.offset && a.outline == b.outline;
    }

    void xapply() override
    {
        STDMaterial::xapply();

        const AffineTransform& tr = STDRenderer::getCurrent()->getTransform();
        float scale = sqrt(tr.a * tr.a + tr.c * tr.c);
        float contrast = 3.0f + scale * 8.0f;

        Vector4 sdfParams(offset, contrast, outline, contrast);
        IVideoDriver::instance->setUniform("sdf_params", sdfParams);

        Vector4 color = outlineColor.toVector();
        IVideoDriver::instance->setUniform("sdf_outline_color", color);

    }
};

UberShaderProgram* SDFMaterial::uberShader = 0;

class TestSignedDistanceFont : public Test
{
public:
    spTextField _txt;
    ResFontBM font;

    spTween t;
    TestSignedDistanceFont()
    {
        SDFMaterial::initMaterial();

        font.initSD("sdf/font.fnt", 8);
        font.load();


        /*how to make SD font
        based on article: https://habrahabr.ru/post/282191/
        1. install ImageMagick
        2. generate huge font with size 400pt, set padding 40x40x40x40
        3. convert font image to SD font image using command below:

        convert font.png -filter Jinc ( +clone -negate -morphology Distance Euclidean -level 50%,-50% ) -morphology Distance Euclidean -compose Plus -composite -level 43%,57% -resize 12.5% font.png

        font image will be downscaled 8 times from 4096 to 512

        declare your font in resources.xml:
        <sdfont font="font.fnt" downsample="8" />
        or load it directly from file as I did above
        */



        spTextField txt = new TextField;
        txt->attachTo(_content);
        _txt = txt;

        TextStyle st;
        st.font = &font;
        st.vAlign = TextStyle::VALIGN_MIDDLE;
        st.hAlign = TextStyle::HALIGN_MIDDLE;
        st.color = Color::CornflowerBlue;

        st.multiline = true;

        txt->setStyle(st);
        txt->setColor(Color::CornflowerBlue);
        txt->setText("The quick brown fox jumps over the lazy dog. 1234567890.");
        txt->setPosition(getStage()->getSize() / 2);
        txt->setWidth(getStage()->getWidth() / 2);
        txt->setAnchor(0.5f, 0.5f);
        txt->addTween(Actor::TweenRotationDegrees(360), 10000, -1);

        spSDFMaterial sdf = new SDFMaterial;

        txt->setMaterial(sdf);


        addButton("scale+", "scale+");
        addButton("scale-", "scale-");

        addButton("weight+", "weight+");
        addButton("weight-", "weight-");

        addButton("outline+", "outline+");
        addButton("outline-", "outline-");
    }

    ~TestSignedDistanceFont()
    {
        SDFMaterial::freeMaterial();
    }

    void clicked(string id)
    {
        spSDFMaterial mat = safeSpCast<SDFMaterial>(_txt->_mat);

        if (id == "outline+")
            mat->outline += 0.01f;
        if (id == "outline-")
            mat->outline -= 0.01f;

        if (id == "scale+")
            _txt->addTween(TweenScale(_txt->getScale() * 1.5f), 300);
        if (id == "scale-")
            _txt->addTween(TweenScale(_txt->getScale() / 1.5f), 300);

        if (id == "weight+")
            mat->offset += 0.01f;
        if (id == "weight-")
            mat->offset -= 0.01f;

        _txt->setMaterial(mat);

    }

    void toggleClicked(string id, const toggle* data)
    {

    }
};

