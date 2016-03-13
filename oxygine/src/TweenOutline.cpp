#include "TweenOutline.h"
#include "STDMaterial.h"
#include "Actor.h"
#include "RenderState.h"

#include "core/gl/VertexDeclarationGL.h"
#include "core/gl/ShaderProgramGL.h"
#include "core/file.h"


namespace oxygine
{



    class TweenOutlineImpl : public TweenPostProcess
    {
    public:
        Color _color;
        int _downsample;

        TweenOutlineImpl(const Color& c, const PostProcessOptions& opt) : TweenPostProcess(opt), _color(c), _downsample(1) {}

        void render(Actor* actor, const RenderState& rs) OVERRIDE
        {
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();

            RectF src(0, 0,
            _pp._screen.getWidth() / (float)_pp._rt->getWidth() / _downsample,
            _pp._screen.getHeight() / (float)_pp._rt->getHeight() / _downsample);

            RectF dest = _pp._screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);


            AffineTransform tr = _pp._transform * _actor->computeGlobalTransform();
            renderer->setTransform(tr);
            renderer->beginElementRendering(true);
            Color color = Color(Color::White).withAlpha(255).premultiplied();
            renderer->drawElement(_pp._rt, color.rgba(), src, dest);
            renderer->drawBatch();


            RenderState r = rs;
            r.material = mat;
            actor->setMaterial(_prevMaterial);
            actor->render(r);
            actor->setMaterial(this);
        }

        void _renderPP() OVERRIDE
        {
            IVideoDriver* driver = IVideoDriver::instance;

            const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2::FORMAT));

            if (!PostProcess::shaderBlurH)
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


                PostProcess::shaderBlurV = new ShaderProgramGL(ShaderProgramGL::createProgram(v, ps, decl));
                driver->setShaderProgram(PostProcess::shaderBlurV);
                driver->setUniformInt("s_texture", 0);

                PostProcess::shaderBlurH = new ShaderProgramGL(ShaderProgramGL::createProgram(h, ps, decl));
                driver->setShaderProgram(PostProcess::shaderBlurH);
                driver->setUniformInt("s_texture", 0);


                file::buffer vs_blit;
                file::buffer fs_blit;
                file::read("pp_blit_vs.glsl", vs_blit);
                file::read("pp_blit_fs.glsl", fs_blit);

                vs_blit.push_back(0);
                fs_blit.push_back(0);


                unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_blit.front(), "", "");
                unsigned int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blit.front(), "", "");

                PostProcess::shaderBlit = new ShaderProgramGL(ShaderProgramGL::createProgram(vs, fs, decl));
                driver->setShaderProgram(PostProcess::shaderBlit);
                driver->setUniformInt("s_texture", 0);
            }


            int w = _pp._screen.size.x;
            int h = _pp._screen.size.y;


            driver->setState(IVideoDriver::STATE_BLEND, 0);

            _downsample = 1;


            spNativeTexture rt = _pp._rt;
            spNativeTexture rt2 = getRTManager().get(0, w, h, _pp._format);

#if 0
            driver->setShaderProgram(PostProcess::shaderBlit);
            pass(rt, Rect(0, 0, w, h), rt2, Rect(0, 0, w / 2, h / 2));

            w /= 2;
            h /= 2;
            _downsample *= 2;
#endif

#if 0

            rt = getRTManager().get(w / 2, h / 2, _pp._format);
            _pp._rt = rt;
            pass(rt2, Rect(0, 0, w, h), rt, Rect(0, 0, w / 2, h / 2));

            w /= 2;
            h /= 2;
            _downsample *= 2;
#endif


            Rect rc(0, 0, w, h);


            driver->setShaderProgram(PostProcess::shaderBlurH);
            driver->setUniform("step", 1.0f / rt->getWidth());
            pass(rt, rc, rt2, rc);


            int alpha = lerp(255, 0, _progress);

            Color c = _color.withAlpha(alpha).premultiplied();

            driver->setShaderProgram(PostProcess::shaderBlurV);
            driver->setUniform("step", 1.0f / rt2->getHeight());

            pass(rt2, rc, rt, rc, c);
        }
    };


    TweenOutline::TweenOutline(const Color& color, const PostProcessOptions& opt) : TweenProxy(new TweenOutlineImpl(color, opt))
    {
    }
}