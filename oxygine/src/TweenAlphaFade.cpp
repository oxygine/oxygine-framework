#include "TweenAlphaFade.h"
#include "core/file.h"
#include "core/gl/ShaderProgramGL.h"
#include "core/gl/oxgl.h"
#include "core/gl/VertexDeclarationGL.h"
#include "core/gl/NativeTextureGLES.h"

namespace oxygine
{
    ShaderProgram* TweenGlow::shaderBlurV = 0;
    ShaderProgram* TweenGlow::shaderBlurH = 0;
    ShaderProgram* TweenGlow::shaderBlit = 0;


    void TweenGlow::render2texture()
    {
        TweenPostProcess::render2texture();
        const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2::FORMAT));

        if (!shaderBlurH)
        {
            file::buffer vs_h;
            file::buffer vs_v;
            file::buffer fs_blur;
            file::read("pp_hblur_vs.glsl", vs_h);
            file::read("pp_vblur_vs.glsl", vs_v);
            file::read("pp_rast_fs.glsl", fs_blur);

            vs_h.push_back(0);
            vs_v.push_back(0);
            fs_blur.push_back(0);


            unsigned int h = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_h.front(), "", "");
            unsigned int v = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_v.front(), "", "");
            unsigned int ps = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blur.front(), "", "");


            shaderBlurV = new ShaderProgramGL(ShaderProgramGL::createProgram(v, ps, decl));
            shaderBlurH = new ShaderProgramGL(ShaderProgramGL::createProgram(h, ps, decl));



            file::buffer vs_blit;
            file::buffer fs_blit;
            file::read("pp_blit_vs.glsl", vs_blit);
            file::read("pp_blit_fs.glsl", fs_blit);

            vs_blit.push_back(0);
            fs_blit.push_back(0);


            unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_blit.front(), "", "");
            unsigned int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blit.front(), "", "");

            shaderBlit = new ShaderProgramGL(ShaderProgramGL::createProgram(vs, fs, decl));
        }


        GLint prevFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

        Rect vp = _screen;
        vp.pos = Point(0, 0);

        vertexPCT2 v[4];



        int w = _screen.size.x;
        int h = _screen.size.y;

        IVideoDriver* driver = IVideoDriver::instance;

        driver->setState(IVideoDriver::STATE_BLEND, 0);

        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt2.get())->getFboID());
        driver->setViewport(Rect(0, 0, w / 2, h / 2));
        driver->setShaderProgram(shaderBlit);
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt);


        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt->getWidth(), h / (float)_rt->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);
        w /= 2;
        h /= 2;


        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt.get())->getFboID());
        driver->setViewport(Rect(0, 0, w / 2, h / 2));
        driver->setShaderProgram(shaderBlit);
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt2);


        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt2->getWidth(), h / (float)_rt2->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);
        w /= 2;
        h /= 2;




        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt2.get())->getFboID());

        driver->setViewport(Rect(0, 0, w, h));
        driver->clear(0);
        driver->setShaderProgram(shaderBlurH);
        driver->setUniform("step", 1.0f / _rt->getWidth());
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt);



        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt->getWidth(), h / (float)_rt->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);



        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt.get())->getFboID());


        driver->setViewport(Rect(0, 0, w, h));
        driver->clear(Color::Red);

        driver->setShaderProgram(shaderBlurV);
        driver->setUniform("step", 1.0f / _rt2->getHeight());
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt2);

        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt2->getWidth(), h / (float)_rt2->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);




        oxglBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }

    void TweenGlow::rtCreated()
    {
        TweenPostProcess::rtCreated();
        _rt2 = IVideoDriver::instance->createTexture();
        _rt2->init(_rt->getWidth() / 2, _rt->getHeight() / 2, TF_R8G8B8A8, true);
    }
}