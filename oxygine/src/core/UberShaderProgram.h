#pragma once
#include <vector>
#include "Restorable.h"
#include "Object.h"
//#include <unordered_map>

namespace oxygine
{
    class ShaderProgram;
    class IVideoDriver;
    DECLARE_SMART(NativeTexture, spNativeTexture);


    class UberShaderProgramBase: public Restorable
    {
    public:
        struct shader
        {
            shader(): program(0), flags(0) {}
            ShaderProgram* program;
            int flags;
        };

        UberShaderProgramBase();
        ~UberShaderProgramBase();

        void init(const std::vector<unsigned char>& baseShader, const char* prepend = "", const char* append = "");

        void release();

        typedef Closure<void (IVideoDriver*, ShaderProgram*)> ShaderUniformsCallback;

        ShaderUniformsCallback  getShaderUniformsCallback() const { return _cb; }
        void setShaderUniformsCallback(ShaderUniformsCallback cb) {_cb = cb;}


        virtual shader*         getShaderProgram(int flags) = 0;


    protected:
        void* _getRestorableObject() {return this;}
        void _restore(Restorable*, void*);

        std::vector<unsigned char> _data;
        ShaderUniformsCallback _cb;

        virtual void releaseShaders() {}
    };

    class UberShaderProgram : public UberShaderProgramBase
    {
    public:
        UberShaderProgram();
        ~UberShaderProgram();

        enum
        {
            ALPHA_PREMULTIPLY = 1,
            SEPARATE_ALPHA = 1 << 1,
            MASK_R_CHANNEL = 1 << 2,
            MASK = 1 << 3,
            SDF = 1 << 4,
            SDF_OUTLINE = 1 << 1,//same as SEPARATE_ALPHA
            _SIZE = 1 << 5
        };

        enum
        {
            SAMPLER_BASE,
            SAMPLER_ALPHA,
            SAMPLER_MASK,
            SAMPLER_NUM,
        };

        shader*                 getShaderProgram(int flags);

        void apply(IVideoDriver* driver, spNativeTexture base, spNativeTexture alpha);

    protected:
        void releaseShaders() OVERRIDE;
        shader _shaders[_SIZE];
    };
}