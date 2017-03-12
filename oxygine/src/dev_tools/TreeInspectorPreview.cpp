#include "TreeInspectorPreview.h"
#include "TreeInspector.h"
#include "Stage.h"
#include "TextField.h"
#include "res/Resources.h"
#include "RenderState.h"
#include "STDRenderer.h"
#include "STDMaterial.h"
#include "DebugActor.h"
#include "core/NativeTexture.h"

namespace oxygine
{


    Vector2 fitSize(const Vector2& destSize, const Vector2& src)
    {
        Vector2 srcSize = src;
        if (srcSize.x < 1)
            srcSize.x = 1;
        if (srcSize.y < 1)
            srcSize.y = 1;

        float scaleX = srcSize.x / destSize.x;
        float scaleY = srcSize.y / destSize.y;

        scaleX = 1.0f / scaleX;
        scaleY = 1.0f / scaleY;

        float scale = scaleX < scaleY ? scaleX : scaleY;
        return Vector2(srcSize.x * scale, srcSize.y * scale);
    }


    TreeInspectorPreview::TreeInspectorPreview(): _drawChildren(false)
    {

        //setTouchEnabled(false);
        //addEventListener(TouchEvent::TOUCH_DOWN, )
    }

    TreeInspectorPreview::~TreeInspectorPreview()
    {

    }

    void TreeInspectorPreview::doUpdate(const UpdateState& us)
    {
        if (getPressed())
        {

        }
        else
        {

        }
    }

    bool _render2cache = false;

    Vector2 TreeInspectorPreview::render2cache(spActor item, const Vector2& size, bool child)
    {
        Material::setCurrent(0);

        _render2cache = true;

        VideoDriverCache& cache = _videoCache;


        STDRenderer* original = STDRenderer::getCurrent();

        STDRenderer renderer(&cache);
        //STDRenderer::current = &renderer;
        renderer.setViewProjTransform(STDRenderer::instance->getViewProjection());
        //renderer.begin();
        STDRenderer::current = &renderer;
        //STDMaterial mat;

        RenderState rs;
        rs.material = STDMaterial::instance;
        RectF clip = RectF::huge();
        rs.clip = &clip;

        if (child)
        {
            bool vis = item->getVisible();
            item->setVisible(true);
            rs.transform = rs.transform * item->getTransformInvert();
            item->render(rs);
            item->setVisible(vis);

            //STDMaterial::instance = originalMat;
        }
        else
            item->doRender(rs);


        Material::setCurrent(0);


        //setSize(30, 30);

        RectF itemRect = cache._bounds;
        if (itemRect == RectF::invalidated())
        {
            itemRect = item->getDestRect();
            if (itemRect.isEmpty())
                itemRect.setSize(Vector2(10, 4));
        }

        Vector2 ns = fitSize(size, itemRect.size);
        float scale = ns.x / itemRect.size.x;

        AffineTransform transform;
        transform.identity();
        transform.scale(Vector2(scale, scale));
        transform.translate(-itemRect.pos);

        cache.transform(transform);

        STDRenderer::current = original;

        //OX_ASSERT(STDRenderer::current);
        _render2cache = false;
        return ns;
    }

    void TreeInspectorPreview::init(spActor item, const Vector2& size, bool tree)
    {
        Vector2 ns = render2cache(item, size, tree);

        AnimationFrame fr = DebugActor::resSystem->getResAnim("checker")->getFrame(0);

        Vector2 uv = ns;

        RectF srcRect = fr.getSrcRect();

        uv.x = std::min(uv.x, fr.getDestRect().getWidth());
        uv.y = std::min(uv.y, fr.getDestRect().getHeight());

        const Diffuse& df = fr.getDiffuse();
        srcRect.size.x = uv.x / (float)df.base->getWidth();
        srcRect.size.y = uv.y / (float)df.base->getHeight();

        RectF destRect = fr.getDestRect();
        destRect.size = ns;

        AnimationFrame cfr;
        cfr.init(0, df, srcRect, destRect, ns);

        setAnimFrame(cfr);
    }

    void TreeInspectorPreview::doRender(RenderState const& parentRenderState)
    {
        Sprite::doRender(parentRenderState);
        Material::setCurrent(0);

        //STDMaterial::instance->getRenderer()->drawBatch();
        _videoCache.render(parentRenderState.transform);
        //STDMaterial::instance->getRenderer()->drawBatch();
        //STDMaterial::instance->getRenderer()->resetSettings();
    }

    VideoDriverCache::VideoDriverCache()
    {
        _batches.push_back(cached_batch());
        rt = new NativeTextureNull;
        _bounds = RectF::invalidated();
    }

    VideoDriverCache::~VideoDriverCache()
    {

    }

    oxygine::VideoDriverCache::cached_batch& VideoDriverCache::current()
    {
        return _batches.back();
    }

    const oxygine::VideoDriverCache::cached_batch& VideoDriverCache::current() const
    {
        return _batches.back();
    }

    oxygine::spNativeTexture VideoDriverCache::createTexture()
    {
        return 0;
    }

    void VideoDriverCache::begin(const Rect& viewport, const Color* color)
    {

    }

    const oxygine::VertexDeclaration* VideoDriverCache::getVertexDeclaration(bvertex_format fmt) const
    {
        return instance->getVertexDeclaration(fmt);
    }

    void VideoDriverCache::setDefaultSettings()
    {

    }

    void VideoDriverCache::setRenderTarget(spNativeTexture r)
    {
        rt = r;
    }

    oxygine::spNativeTexture VideoDriverCache::getRenderTarget() const
    {
        return rt;
    }

    void VideoDriverCache::setShaderProgram(ShaderProgram* program)
    {
        current().program = program;
    }

    void VideoDriverCache::setTexture(int sampler, spNativeTexture texture)
    {
        current().textures[sampler] = texture;
    }

    void VideoDriverCache::setState(STATE state, unsigned int value)
    {
        current().states[state] = value;
    }

    void VideoDriverCache::addUni(const char* id, cached_batch::uni::type tp, const void* ptr, int sz)
    {
        cached_batch::uni uni;
        uni.id = id;
        uni.tp = tp;
        const unsigned char* data = (const unsigned char*)ptr;
        uni.data.assign(data, data + sz);
        current().uniforms.push_back(uni);
    }

    void VideoDriverCache::setUniformInt(const char* id, int v)
    {
        addUni(id, cached_batch::uni::uni_int, &v, sizeof(v));
    }

    void VideoDriverCache::setUniform(const char* id, float v)
    {
        addUni(id, cached_batch::uni::uni_float, &v, sizeof(v));
    }

    void VideoDriverCache::setUniform(const char* id, const Matrix* v, int num)
    {
        addUni(id, cached_batch::uni::uni_matrix, v, sizeof(*v) * num);
        wvp = *v;
    }

    void VideoDriverCache::setUniform(const char* id, const Vector2* v, int num)
    {
        addUni(id, cached_batch::uni::uni_vec2, v, sizeof(*v) * num);
    }

    void VideoDriverCache::setUniform(const char* id, const Vector3* v, int num)
    {
        addUni(id, cached_batch::uni::uni_vec3, v, sizeof(*v) * num);
    }

    void VideoDriverCache::setUniform(const char* id, const Vector4* v, int num)
    {
        addUni(id, cached_batch::uni::uni_vec4, v, sizeof(*v) * num);
    }

    void VideoDriverCache::setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest)
    {
        current().blendSrc = src;
        current().blendDest = dest;
    }

    void VideoDriverCache::setScissorRect(const Rect* r)
    {
        if (r)
            current().scissor = *r;
        else
            current().scissor = EmptyScissor;

    }

    bool VideoDriverCache::getScissorRect(Rect& r) const
    {
        if (current().scissor == EmptyScissor)
            return false;
        r = current().scissor;
        return true;
    }

    void VideoDriverCache::nextBatch()
    {
        vertexPCT2* v = (vertexPCT2*)(&current().vertices.front());

        size_t num = current().vertices.size() / current().vdecl->size;


        for (size_t i = 0; i != num; ++i)
        {
            v = (vertexPCT2*)(&current().vertices.front() + current().vdecl->size * i);
            Vector4 p(v->x, v->y, v->z, 1.0f);
            p = wvp.transformVec4(p);
            p.x /= p.w;
            p.y /= p.w;
            v->x = p.x * getStage()->getWidth();
            v->y = getStage()->getHeight() - p.y * getStage()->getHeight();
            RectF f(v->x, v->y, 0, 0);
            _bounds.unite(f);
        }


        _batches.push_back(cached_batch());
        cached_batch& b = _batches.back();
        b = _batches[_batches.size() - 2];
        b.indices.clear();
        b.vertices.clear();
        b.numIndices = 0;
        b.numVertices = 0;
    }

    void VideoDriverCache::draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int numVertices, const unsigned short* indicesData, unsigned int numIndices)
    {
        if (!numIndices)
            return;

        current().vdecl = decl;
        current().pt = pt;
        current().numVertices = numVertices;
        current().numIndices = numIndices;
        current().vertices.assign((const char*)verticesData, (const char*)verticesData + decl->size * numVertices);
        current().indices.assign(indicesData, indicesData + numIndices);

        nextBatch();
    }

    void VideoDriverCache::draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData, unsigned int numVertices)
    {
        current().vdecl = decl;
        current().pt = pt;
        current().numVertices = numVertices;
        current().vertices.assign((const char*)verticesData, (const char*)verticesData + current().vdecl->size * numVertices);
        nextBatch();
    }

    void VideoDriverCache::render(const AffineTransform& transform)
    {
        for (batches::iterator i = _batches.begin(); i != _batches.end(); ++i)
        {
            const cached_batch& b = *i;

            std::vector<char> modified = b.vertices;

            if (b.vertices.size())
            {
                size_t num = b.vertices.size() / b.vdecl->size;

                //if (0)
                for (size_t i = 0; i != num; ++i)
                {
                    vertexPCT2* v = (vertexPCT2*)(&modified.front() + b.vdecl->size * i);
                    Vector2 np = transform.transform(Vector2(v->x, v->y));
                    v->x = np.x;
                    v->y = np.y;
                }
            }

            for (int i = 0; i < cached_batch::MAX_TEXTURES; ++i)
                instance->setTexture(i, b.textures[i]);

            if (b.program)
                instance->setShaderProgram(b.program);
            instance->setBlendFunc(b.blendSrc, b.blendDest);
            for (int i = 0; i < STATE_NUM; ++i)
                instance->setState((STATE)i, b.states[i]);

            /*
            if (b.scissor == EmptyScissor)
                instance->setScissorRect(0);
            else
                instance->setScissorRect(&b.scissor);

            */
            //if(0)
            for (size_t i = 0; i < b.uniforms.size(); ++i)
            {
                const cached_batch::uni& uni = b.uniforms[i];
                switch (uni.tp)
                {
                    case cached_batch::uni::uni_float:
                        instance->setUniform(uni.id.c_str(), *((const float*)&uni.data[0])); break;
                    case cached_batch::uni::uni_int:
                        instance->setUniformInt(uni.id.c_str(), *((const int*)&uni.data[0])); break;
                    case cached_batch::uni::uni_matrix:
                    {
                        Matrix m = STDRenderer::instance->getViewProjection();
                        //instance->setUniform(uni.id.c_str(), ((const Matrix*)&uni.data[0]));
                        instance->setUniform(uni.id.c_str(), m);
                        break;
                    }
                    case cached_batch::uni::uni_vec2:
                        instance->setUniform(uni.id.c_str(), ((const Vector2*)&uni.data[0]), int(uni.data.size() / sizeof(Vector2))); break;
                    case cached_batch::uni::uni_vec3:
                        instance->setUniform(uni.id.c_str(), ((const Vector3*)&uni.data[0]), int(uni.data.size() / sizeof(Vector3))); break;
                    case cached_batch::uni::uni_vec4:
                        instance->setUniform(uni.id.c_str(), ((const Vector4*)&uni.data[0]), int(uni.data.size() / sizeof(Vector4))); break;
                    default:
                        break;
                }
            }

            if (b.numIndices)
                instance->draw(b.pt, b.vdecl, &modified.front(), b.numVertices, &b.indices.front(), b.numIndices);
            else
            {
                if (b.numVertices)
                    instance->draw(b.pt, b.vdecl, &modified.front(), b.numVertices);
            }
        }
    }

    void VideoDriverCache::transform(const AffineTransform& transform)
    {
        /*
        AffineTransform transform;
        transform.identity();
        */

        for (batches::iterator i = _batches.begin(); i != _batches.end(); ++i)
        {
            cached_batch& b = *i;
            if (!b.numVertices)
                continue;

            size_t num = b.vertices.size() / b.vdecl->size;

            std::vector<char>& modified = b.vertices;
            //if (0)
            for (size_t i = 0; i != num; ++i)
            {
                vertexPCT2* v = (vertexPCT2*)(&modified.front() + b.vdecl->size * i);
                Vector2 np = transform.transform(Vector2(v->x, v->y));
                v->x = np.x;
                v->y = np.y;
            }

            /*
            if (b.scissor != EmptyScissor)
            {
                Vector2 lt = b.scissor.getLeftTop().cast<Vector2>();
                lt = m.transform(lt);

                Vector2 rb = b.scissor.getRightBottom().cast<Vector2>();
                rb = m.transform(rb);

                Vector2 size = rb - lt;
                b.scissor = RectF(lt, size).cast<Rect>();
            }
            */
        }
    }

}
