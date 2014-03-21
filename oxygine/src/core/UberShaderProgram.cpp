#include "UberShaderProgram.h"
#include "vertex.h"
#include "gl/VideoDriverGLES20.h"
#include "gl/ShaderProgramGL.h"

namespace oxygine
{
	UberShaderProgram::UberShaderProgram()
	{

	}

	void UberShaderProgram::init(const std::vector<unsigned char> &baseShader, const char *prepend, const char *append)		
	{
		_data = baseShader;

		_data.insert(_data.begin(), prepend, prepend + strlen(prepend));
		_data.insert(_data.end(), append, append + strlen(append));
		_data.push_back(0);

		reg(CLOSURE(this, &UberShaderProgram::_restore), 0);
	}

	void UberShaderProgram::releaseShaders()
	{
		for (int i = 0; i < SIZE; ++i)
		{
			shader &s = _shaders[i];
			delete s.program;
			s.program = 0;
		}
	}

	UberShaderProgram::~UberShaderProgram()
	{
		releaseShaders();
	}

	void UberShaderProgram::_restore(Restorable *, void*)
	{

	}

	void UberShaderProgram::release()
	{
		for (int i = 0; i < SIZE; ++i)
		{
			shader &s = _shaders[i];
			if (s.program)
			{
				delete s.program;
				s.program = 0;
			}			
		}

		unreg();
	}

	UberShaderProgram::shader *UberShaderProgram::getShaderProgram(int flags)
	{
		shader &s = _shaders[flags];

		if (!s.program)
		{
			bvertex_format bformat = VERTEX_PCT2;
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
				bformat = VERTEX_PCT2T2;
			}

			char *end = prepend + strlen(prepend);
			strcat(prepend, "#define program_main_ps main\n");
			strcat(prepend, "#define PS\n");

			int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, data, prepend, append);

			*end = 0;
			strcat(prepend, "#define program_main_vs main\n");
			strcat(prepend, "#define VS\n");

			int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, data, prepend, append);					

			ShaderProgramGL *pgl = new ShaderProgramGL;
			VideoDriverGLES20 *driver = ((VideoDriverGLES20*)IVideoDriver::instance);
			const VertexDeclarationGL *decl = driver->getVertexDeclaration(bformat);
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
}