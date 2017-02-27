#pragma once

#include "test.h"
#include "core/UberShaderProgram.h"
#include "core/gl/VideoDriverGLES20.h"
#include "core/gl/oxgl.h"
#include "core/gl/ShaderProgramGL.h"


class LightningMaterial : public Material
{
public:
    const VertexDeclaration* _vdecl;
    ShaderProgramGL* _lightShader;
    Vector2 _light;

    vector<vertexPCT2T2> _vertices;
    AnimationFrame _normal;
    spNativeTexture _base;

    LightningMaterial()
    {
        _normal = resources.getResAnim("normal")->getFrame(0);

        _light = getStage()->getSize() / 2;

        IVideoDriver* driver = IVideoDriver::instance;
        _vdecl = driver->getVertexDeclaration(vertexPCT2T2::FORMAT);

        //load vertex shader
        file::buffer vsdata;
        file::read("light_vs.glsl", vsdata);
        vsdata.push_back(0);///null terminating string
        unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vsdata.front(), "", "");

        //load fragment shader
        file::buffer fsdata;
        file::read("light_fs.glsl", fsdata);
        fsdata.push_back(0);///null terminating string
        unsigned int ps = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fsdata.front(), "", "");

        //link into 1 shader program
        unsigned int pr = ShaderProgramGL::createProgram(vs, ps, static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2T2::FORMAT)));
        _lightShader = new ShaderProgramGL(pr);

        //set shader and apply samplers uniforms
        driver->setShaderProgram(_lightShader);
        driver->setUniformInt("base_texture", 0);
        driver->setUniformInt("normal_texture", 1);
    }

    void apply(Material* prev) override
    {
        IVideoDriver* driver = IVideoDriver::instance;
        driver->setShaderProgram(_lightShader);

        Vector2 light = getStage()->getDescendant("light")->getPosition();

        driver->setUniform("light", &light, 1);
        driver->setUniform("mat", &STDMaterial::instance->getRenderer()->getViewProjection());

        driver->setTexture(1, _normal.getDiffuse().base);

        driver->setState(IVideoDriver::STATE_BLEND, 0);
        driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
        driver->setBlendFunc(IVideoDriver::BT_SRC_ALPHA, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
    }

    void finish() override
    {
        drawBatch(IVideoDriver::instance);
    }

    void drawBatch(IVideoDriver* driver)
    {
        size_t count = _vertices.size();
        if (!count)
            return;

        size_t indices = (count * 3) / 2;

        driver->setTexture(0, _base);
        driver->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_vertices.front(), (unsigned int)count, &STDRenderer::indices16.front(), (unsigned int)indices);

        _vertices.clear();
    }


    void doRender(Sprite* sprite, const RenderState& rs) override
    {
        Material::setCurrent(this);

        vertexPCT2T2 v[4];
        const AnimationFrame& frame = sprite->getAnimFrame();

        if (frame.getDiffuse().base != _base)
        {
            drawBatch(IVideoDriver::instance);
            _base = frame.getDiffuse().base;
        }

        fillQuadT2(v, frame.getSrcRect(), _normal.getSrcRect(), sprite->getDestRect(), rs.transform, 0xffFFffFF);
        _vertices.insert(_vertices.end(), v, v + 4);
    }
};

class TestUserShader2 : public Test
{
public:
    Draggable drag;

    TestUserShader2()
    {
        LightningMaterial* mat = new LightningMaterial;

        spSprite spr = new Sprite();
        spr->setResAnim(resources.getResAnim("tiled2"));
        spr->setScale(2);
        spr->setAnchor(0.5f, 0.5f);
        spr->setPosition(content->getSize() / 2);
        spr->attachTo(content);
        spr->setMaterial(mat);


        spr = new Sprite();
        spr->setResAnim(resources.getResAnim("tiled2"));
        spr->setScale(2);
        spr->setAnchor(0.5f, 0.5f);
        spr->setPosition(content->getSize() / 2 + Vector2(300, 50));
        spr->attachTo(content);
        spr->setMaterial(mat);

        spr = new Sprite();
        spr->setResAnim(resources.getResAnim("tiled2"));
        spr->setScale(2);
        spr->setAnchor(0.5f, 0.5f);
        spr->setPosition(content->getSize() / 2 + Vector2(-200, 50));
        spr->attachTo(content);
        spr->setMaterial(mat);


        spSprite light = new Sprite;
        light->setName("light");
        light->setResAnim(resources.getResAnim("light"));
        light->setAnchor(0.5f, 0.5f);
        drag.init(light.get());
        light->setPosition(getSize() / 2);

        content->addChild(light);
    }

};