#pragma once
#include "oxygine_include.h"
#include "Object.h"

namespace oxygine
{
	class ShaderProgram: public Object
	{
	public:
		//virtual void bind() = 0;

		virtual unsigned int getID() const = 0;

		virtual void setUniform(const char *id, const Vector4 *v, int num) = 0;
		virtual void setUniform(const char *id, const Matrix *) = 0;
		virtual void setUniform(const char *id, float v) = 0;
		virtual void setUniformInt(const char *id, int v) = 0;
	};
}