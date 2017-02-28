#pragma once
#include "oxygine-include.h"
#include "Sprite.h"
#include "core/VertexDeclaration.h"

namespace oxygine
{
    const Rect EmptyScissor(0, 0, -1, -1);

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
            cached_batch(): program(0), vdecl(0), numVertices(0), numIndices(0), blendSrc(IVideoDriver::BT_ONE), blendDest(IVideoDriver::BT_ONE), scissor(EmptyScissor)
            {
                memset(states, 0, sizeof(states));
            }
            ShaderProgram* program;

            enum {MAX_TEXTURES = 16};
            spNativeTexture textures[MAX_TEXTURES];

            const VertexDeclaration* vdecl;
            PRIMITIVE_TYPE pt;
            std::vector<char> vertices;
            std::vector<unsigned short> indices;

            struct uni
            {
                std::string id;
                enum type {uni_int, uni_float, uni_vec2, uni_vec3, uni_vec4, uni_matrix};
                type tp;
                std::vector<unsigned char> data;

            };
            std::vector<uni> uniforms;


            int numVertices;
            int numIndices;
            unsigned int states[STATE_NUM];
            BLEND_TYPE blendSrc, blendDest;
            Rect scissor;
        };

        typedef std::vector<cached_batch> batches;
        batches _batches;
        RectF _bounds;

        AffineTransform _transform;
        mutable VertexDeclarations<VertexDeclarationNull> _declarations;

        VideoDriverCache();
        ~VideoDriverCache();

        cached_batch& current();
        const cached_batch& current() const;

        spNativeTexture createTexture() override;
        void begin(const Rect& viewport, const Color* color) override;
        const VertexDeclaration* getVertexDeclaration(bvertex_format fmt) const override;
        void setDefaultSettings() override;
        void setRenderTarget(spNativeTexture) override;
        spNativeTexture getRenderTarget() const override;
        void setShaderProgram(ShaderProgram* program) override;
        void setTexture(int sampler, spNativeTexture texture) override;
        void setState(STATE state, unsigned int value) override;

        void addUni(const char* id, cached_batch::uni::type tp, const void* ptr, int sz);

        void setUniformInt(const char* id, int v) override;
        void setUniform(const char* id, const Vector4* v, int num) override;
        void setUniform(const char* id, const Vector3* v, int num) override;
        void setUniform(const char* id, const Vector2* v, int num) override;
        void setUniform(const char* id, const Matrix* v) override;
        void setUniform(const char* id, float v) override;
        void setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest) override;
        void setScissorRect(const Rect*) override;
        bool getScissorRect(Rect&) const override;

        void nextBatch();

        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData,  unsigned int numVertices) override;
        void draw(PRIMITIVE_TYPE pt, const VertexDeclaration* decl, const void* verticesData,  unsigned int numVertices, const unsigned short* indicesData, unsigned int numIndices) override;
        void render(const AffineTransform& m);
        void transform(const AffineTransform& m);

        spNativeTexture rt;
        Matrix wvp;
    };


    class TreeInspectorPreview: public Sprite
    {
    public:
        TreeInspectorPreview();
        ~TreeInspectorPreview();

        void init(spActor item, const Vector2& size, bool tree);

        void doRender(const RenderState& parentRenderState) override;

    private:
        void doUpdate(const UpdateState& us) override;
        friend class TreeInspector;
        //bool _onEvent(const EventState &es);

        Vector2 render2cache(spActor item, const Vector2& size,  bool child);

        VideoDriverCache _videoCache;


        Rect _getItemRect();
        bool _drawChildren;
    };
}