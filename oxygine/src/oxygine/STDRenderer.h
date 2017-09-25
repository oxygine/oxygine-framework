#pragma once
#include "oxygine-include.h"
#include "core/Renderer.h"
#include "MaterialX.h"

namespace oxygine
{
    class RenderStateCache
    {
    public:
        RenderStateCache(IVideoDriver* d);
        


        const spNativeTexture& getTexture(int sampler) { return _textures[sampler]; }

        void setTexture(int sampler, const spNativeTexture& t);
        void setBlendMode(blend_mode blend);
        bool setShader(ShaderProgram* prog);

        void reset();
        void resetTextures();

        void changeDriver(IVideoDriver *d);//used for DebugActor

    protected:
        enum { MAX_TEXTURES = 8 };

        spNativeTexture _textures[MAX_TEXTURES];
        ShaderProgram*  _program;
        IVideoDriver*   _driver;
        blend_mode      _blend;
    };

    RenderStateCache& rsCache();

    class ShaderProgramChangedHook
    {
    public:
        ShaderProgramChangedHook(): prev(0), next(0) {}

        ShaderProgramChangedHook* prev;
        ShaderProgramChangedHook* next;

        std::function< void() > hook;
    };

    class STDRenderer : public ShaderProgramChangedHook
    {
    public:

        static STDRenderer* current;
        static STDRenderer* instance;
        /**Initializes internal classes. Called automatically from oxygine::init();*/
        static void initialize();
        /**Clears internal data*/
        static void release();
        /**for lost context*/
        static void reset();
        /**is Renderer was restored and ready to be used*/
        static bool isReady();
        /**restore after lost context*/
        static void restore();

        /**returns activated renderer with STDRenderer::begin*/
        static STDRenderer* getCurrent();

        /**White 4x4 Texture*/
        static spNativeTexture white;
        static spNativeTexture invisible;

        static UberShaderProgram uberShader;
        static std::vector<unsigned char> uberShaderBody;
        static std::vector<unsigned short> indices16;
        static size_t maxVertices;



        STDRenderer(IVideoDriver* driver = 0);
        virtual ~STDRenderer();

        const Matrix&               getViewProjection() const;
        IVideoDriver*               getDriver();
        const AffineTransform&      getTransform() const { return _transform; }
        const VertexDeclaration*    getVertexDeclaration() const { return _vdecl; }
        unsigned int                getBaseShaderFlags() const { return _baseShaderFlags; }

        void setShaderFlags(unsigned int);
        void setViewProj(const Matrix& viewProj);
        void setVertexDeclaration(const VertexDeclaration* decl);
        void setUberShaderProgram(UberShaderProgram* pr);
        void setBaseShaderFlags(unsigned int fl);

        /**Sets World transformation.*/
        void setTransform(const Transform& world);
        void addQuad(const Color&, const RectF& srcRect, const RectF& destRect);

        /**Begins rendering into RenderTexture or into primary framebuffer if rt is null*/
        void begin();
        void begin(spNativeTexture renderTarget, const Rect* viewport = 0);
        /**Completes started rendering and restores previous Frame Buffer.*/
        void end();
        /**initializes View + Projection matrices where TopLeft is (0,0) and RightBottom is (width, height). use flipU = true for render to texture*/
        void initCoordinateSystem(int width, int height, bool flipU = false);

        /**Draws existing batch immediately.*/
        void flush();

        virtual void addVertices(const void* data, unsigned int size);


        //debug utils
#ifdef OXYGINE_DEBUG_T2P
        static void showTexel2PixelErrors(bool show);
#endif

        void swapVerticesData(std::vector<unsigned char>& data);
        void swapVerticesData(STDRenderer& r);

        OXYGINE_DEPRECATED
        void setViewProjTransform(const Matrix& viewProj);

        void pushShaderSetHook(ShaderProgramChangedHook* hook);
        void popShaderSetHook();

        bool isEmpty() const { return _verticesData.empty(); }

    protected:
        virtual void shaderProgramChanged() {}

        Transform _transform;

        void setShader(ShaderProgram* prog);

        void xdrawBatch();


        void xaddVertices(const void* data, unsigned int size);
        void checkDrawBatch();

        std::vector<unsigned char> _verticesData;

        const VertexDeclaration* _vdecl;

        IVideoDriver* _driver;
        Matrix _vp;

        virtual void xbegin();

        ShaderProgramChangedHook* _sphookFirst;
        ShaderProgramChangedHook* _sphookLast;

        UberShaderProgram* _uberShader;

        unsigned int _baseShaderFlags;

        spNativeTexture _prevRT;
    };


    typedef void(*render_texture_hook)(const spNativeTexture& nt);
    void set_render_texture_hook(render_texture_hook);
    render_texture_hook get_render_texture_hook();
}