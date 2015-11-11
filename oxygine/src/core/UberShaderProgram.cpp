#include "UberShaderProgram.h"
#include "vertex.h"
#include "gl/VideoDriverGLES20.h"
#include "gl/ShaderProgramGL.h"
#include "NativeTexture.h"

namespace oxygine
{
    UberShaderProgramBase::UberShaderProgramBase()
    {

    }

    void UberShaderProgramBase::init(const std::vector<unsigned char>& baseShader, const char* prepend, const char* append)
    {
        _data = baseShader;

        _data.insert(_data.begin(), prepend, prepend + strlen(prepend));
        _data.insert(_data.end(), append, append + strlen(append));
        _data.push_back(0);

        reg(CLOSURE(this, &UberShaderProgramBase::_restore), 0);
    }

    UberShaderProgramBase::~UberShaderProgramBase()
    {
        releaseShaders();
    }

    void UberShaderProgramBase::_restore(Restorable*, void*)
    {

    }

    void UberShaderProgramBase::release()
    {
        releaseShaders();
        unreg();
    }

    UberShaderProgramBase::shader* UberShaderProgram::getShaderProgram(int flags)
    {
        shader& s = _shaders[flags];

        if (!s.program)
        {
            bvertex_format bformat = vertexPCT2::FORMAT;
            s.flags = flags;
            const char* data = (const char*)&_data.front();

            char prepend[255];
            char append[255];

            append[0] = 0;
            prepend[0] = 0;

            if (flags & ALPHA_PREMULTIPLY)
                strcat(prepend, "#define ALPHA_PREMULTIPLY\n");

            if (flags & SEPARATE_ALPHA)
                strcat(prepend, "#define SEPARATE_ALPHA\n");

            if (flags & MASK_R_CHANNEL)
                strcat(prepend, "#define MASK_R_CHANNEL\n");

            if (flags & MASK)
            {
                strcat(prepend, "#define MASK\n");
            }

            char* end = prepend + strlen(prepend);
            strcat(prepend, "#define program_main_ps main\n");
            strcat(prepend, "#define PS\n");

            int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, data, prepend, append);

            *end = 0;
            strcat(prepend, "#define program_main_vs main\n");
            strcat(prepend, "#define VS\n");

            int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, data, prepend, append);

            ShaderProgramGL* pgl = new ShaderProgramGL;
            VideoDriverGLES20* driver = ((VideoDriverGLES20*)IVideoDriver::instance);
            const VertexDeclarationGL* decl = driver->getVertexDeclaration(bformat);
            pgl->init(ShaderProgramGL::createProgram(vs, fs, decl));


            CHECKGL();

            driver->setShaderProgram(pgl);

            CHECKGL();

            driver->setUniformInt("base_texture", UberShaderProgram::SAMPLER_BASE);
            driver->setUniformInt("alpha_texture", UberShaderProgram::SAMPLER_ALPHA);
            driver->setUniformInt("mask_texture", UberShaderProgram::SAMPLER_MASK);

            s.program = pgl;


            CHECKGL();
        }

        return &s;
    }

    UberShaderProgram::UberShaderProgram()
    {

    }

    UberShaderProgram::~UberShaderProgram()
    {
        releaseShaders();
    }

    void UberShaderProgram::releaseShaders()
    {
        for (int i = 0; i < SIZE; ++i)
        {
            shader& s = _shaders[i];
            delete s.program;
            s.program = 0;
        }
    }

    void UberShaderProgram::apply(IVideoDriver* driver, spNativeTexture base, spNativeTexture alpha)
    {
        driver->setTexture(UberShaderProgram::SAMPLER_BASE, base);

        if (alpha)
            driver->setTexture(UberShaderProgram::SAMPLER_ALPHA, alpha);
    }
}