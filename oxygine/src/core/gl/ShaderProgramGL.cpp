#include "ShaderProgramGL.h"
#include "VertexDeclarationGL.h"
#include "oxgl.h"

namespace oxygine
{
	ShaderProgramGL::ShaderProgramGL():_program(0)
	{

	}

	ShaderProgramGL::~ShaderProgramGL()
	{
		if (_program)
			glDeleteProgram(_program);
        CHECKGL();
	}



	void printShaderInfoLog(GLuint shader)
	{
		GLint length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length)
		{
			char* buffer = (char*)malloc( sizeof(char) * length ) ;
			glGetShaderInfoLog(shader, length, NULL, buffer);
			if (buffer[0])
				log::error("%s", buffer);
			free(buffer);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (success != GL_TRUE) 
			{
				OX_ASSERT(!"shader build error");
				exit(1);
			}
		}
        
        CHECKGL();
	}


	unsigned int ShaderProgramGL::createShader(unsigned int type, const char* data, const char *prepend, const char *append)
	{
		GLuint shader = glCreateShader(type);

		const char *sources[16];
		const char **ptr = &sources[0];

		char nonGLES[] = 
			"#define lowp\n"
			"#define mediump\n"
			"#define highp\n";

#if SDL_VIDEO_OPENGL
		*ptr = nonGLES;
		ptr++;
#endif

		if (prepend)
		{
			*ptr = prepend;
			ptr++;
		}

		*ptr = data;
		ptr++;

		if (append)
		{
			*ptr = append;
			ptr++;
		}

		int num = ptr - sources;
		glShaderSource(shader, num, sources, 0);
		glCompileShader(shader);
		printShaderInfoLog(shader);
        
        CHECKGL();

		return shader;
	}


	unsigned int ShaderProgramGL::createProgram(int vs, int fs, const VertexDeclarationGL *decl)
	{
		int p = glCreateProgram();
		glAttachShader(p, vs);
		glAttachShader(p, fs);

		for (int i = 0; i < decl->numElements; ++i)		
			glBindAttribLocation(p, decl->elements[i].index, decl->elements[i].name);

		glLinkProgram(p);
        
        CHECKGL();

		return p;
	}
}