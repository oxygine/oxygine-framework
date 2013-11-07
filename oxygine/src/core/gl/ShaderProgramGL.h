#pragma once
#include "../ShaderProgram.h"
#include "oxgl.h"
namespace oxygine
{
	class VertexDeclarationGL;
	class ShaderProgramGL: public ShaderProgram
	{
	public:
		ShaderProgramGL();
		~ShaderProgramGL();

		void init(GLuint p)
		{
			_program = p;
		}

		unsigned int getID() const 
		{
			return _program;
		}

		void setUniformInt(const char *id, int v)
		{
			int location = glGetUniformLocation(_program, id);
			if (location == -1)
				return;
			glUniform1i(location, v);
            CHECKGL();
		}

		int getUniformLocation(const char *id) const
		{
			int i = glGetUniformLocation(_program, id);
            CHECKGL();
            return i;
		}

		void setUniform(const char *id, const Vector4 *v, int num)
		{
			int p = glGetUniformLocation(_program, id);
			glUniform4fv(p, num, v->m);
            CHECKGL();
		}

		void setUniform(const char *id, const Matrix *mat)
		{
			int p = glGetUniformLocation(_program, id);
			glUniformMatrix4fv(p, 1, GL_FALSE, mat->ml);
            CHECKGL();
		}

		void setUniform(const char *id, float val)
		{
			int p = glGetUniformLocation(_program, id);
			glUniform1f(p, val);
            CHECKGL();
		}

		static unsigned int createShader(unsigned int type, const char* data, const char *prepend, const char *append);
		static unsigned int createProgram(int vs, int fs, const VertexDeclarationGL *decl);

	private:
		GLuint _program;
	};


}
