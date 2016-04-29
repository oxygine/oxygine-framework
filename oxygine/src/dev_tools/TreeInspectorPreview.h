#pragma once
#include "oxygine_include.h"
#include "Sprite.h"
#include "core/VertexDeclaration.h"

namespace oxygine
{
    class TreeInspector;

    class VertexDeclarationNull: public VertexDeclaration
    {
    public:
        void init(bvertex_format fmt)
        {
            size = getVertexSize(fmt);
            bformat = fmt;
        }
    };

    class VideoDriverCache: public VideoDriverNull
    {
    public:
        struct cached_batch
        {
            cached_batch(): program(0), vdecl(0), indicesShortType(false), numVertices(0), numIndices(0), blendSrc(IVideoDriver::BT_ONE), blendDest(IVideoDriver::BT_ONE)
            {
                memset(states, 0, sizeof(states));
            }
            ShaderProgram* program;

            enum {MAX_TEXTURES = 16};
            spNativeTexture textures[MAX_TEXTURES];

            const VertexDeclaration* vdecl;
            PRIMITIVE_TYPE pt;
            std::vector<char> vertices;
            std::vector<char> indices;
            int numVertices;
            int numIndices;
            unsigned int states[STATE_NUM];
            BLEND_TYPE blendSrc, blendDest;
            bool indicesShortType;
        };

        typedef std::vector<cached_batch> batches;
        batches _batches;
        RectF _bounds;

        AffineTransform _transform;
        mutable VertexDeclarations<VertexDeclarationNull> _declarations;

        VideoDriverCache(): _bounds(0, 0, 0, 0)
        {
            _batches.push_back(cached_batch());
        }

        cached_batch& current()
        {
            return _batches.back();
        }

        spNativeTexture createTexture() {return 0;}

        void begin(const Rect& viewport, const Color* color)
        {
        }

        const VertexDeclaration* getVertexDeclaration(bvertex_format fmt) const
        {
            return instance->getVertexDeclaration(fmt);
        }

        void setDefaultSettings()
        {

        }

        void setRenderTarget(spNativeTexture)
        {

        }

        void setShaderProgram(ShaderProgram* program)
        {
            current().program = program;
        }

        void setTexture(int sampler, spNativeTexture texture)
        {
            current().textures[sampler] = texture;
        }

        void setState(STATE state, unsigned int value)
        {
            current().states[state] = value;
        }

        void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest)
        {
            current().blendSrc = src;
            current().blendDest = dest;
        }

        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData,  unsigned int numVertices)
        {
            current().vdecl = decl;
            current().pt = pt;
            current().numVertices = numVertices;
            current().vertices.assign((const char*)verticesData, (const char*)verticesData + current().vdecl->size * numVertices);
            _batches.push_back(cached_batch());

        }

        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData,  unsigned int numVertices, const void* indicesData, unsigned int numIndices, bool indicesShortType)
        {
            current().vdecl = decl;
            current().pt = pt;
            current().numVertices = numVertices;
            current().numIndices = numIndices;
            current().vertices.assign((const char*)verticesData, (const char*)verticesData + decl->size * numVertices);
            current().indices.assign((const char*)indicesData, (const char*)indicesData + numIndices * (indicesShortType ? 2 : 1));

            const vertexPCT2* v = (const vertexPCT2*)(&current().vertices.front());
            if (_batches.size() == 1)
            {
                OX_ASSERT(current().vertices.size());
                _bounds = RectF(v->x, v->y, 0, 0);
            }

            size_t num = current().vertices.size() / current().vdecl->size;

            for (size_t i = 0; i != num; ++i)
            {
                v = (const vertexPCT2*)(&current().vertices.front() + current().vdecl->size * i);
                RectF f(v->x, v->y, 0, 0);
                _bounds.unite(f);
            }


            _batches.push_back(cached_batch());
        }

        void render(const AffineTransform& transform)
        {
            for (batches::iterator i = _batches.begin(); i != _batches.end(); ++i)
            {
                const cached_batch& b = *i;
                if (!b.program)
                    break;
                if (!b.vdecl)
                    break;

                size_t num = b.vertices.size() / b.vdecl->size;

                std::vector<char> modified = b.vertices;
                for (size_t i = 0; i != num; ++i)
                {
                    vertexPCT2* v = (vertexPCT2*)(&modified.front() + b.vdecl->size * i);
                    Vector2 np = transform.transform(Vector2(v->x, v->y));
                    v->x = np.x;
                    v->y = np.y;
                }

                for (int i = 0; i < cached_batch::MAX_TEXTURES; ++i)
                    instance->setTexture(i, b.textures[i]);

                instance->setShaderProgram(b.program);
                instance->setBlendFunc(b.blendSrc, b.blendDest);
                for (int i = 0; i < STATE_NUM; ++i)
                    instance->setState((STATE)i, b.states[i]);
                if (b.numIndices)
                    instance->draw(b.pt, b.vdecl, &modified.front(), b.numVertices, &b.indices.front(), b.numIndices, b.indicesShortType);
                else
                    instance->draw(b.pt, b.vdecl, &modified.front(), b.numVertices);
            }
        }
    };


    class TreeInspectorPreview: public Sprite
    {
    public:
        TreeInspectorPreview(TreeInspector* tree);
        ~TreeInspectorPreview();

        void init(spActor item);

        void doRender(RenderState const& parentRenderState);

    private:
        friend class TreeInspector;
        //bool _onEvent(const EventState &es);

        VideoDriverCache _videoCache;
        AffineTransform _cacheTransform;

        Rect _getItemRect();

        Actor* _prevParent;

        TreeInspector* _tree;
        bool _drawChildren;
    };
}