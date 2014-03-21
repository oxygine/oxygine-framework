#pragma once
#include <vector>
#include "Restorable.h"

namespace oxygine
{
	class ShaderProgram;
	class IVideoDriver;

	class UberShaderProgram: public Restorable
	{
	public:
		struct shader
		{
			shader():program(0), flags(0){}
			ShaderProgram *program;
			int flags;
		};

		enum
		{
			ALPHA_PREMULTIPLY = 1,
			SEPARATE_ALPHA = 1 << 1,
			MASK_R_CHANNEL = 1 << 2,
			MASK = 1 << 3,
			SIZE = 1 << 4
		};		

		enum 
		{
			SAMPLER_BASE,
			SAMPLER_ALPHA,
			SAMPLER_MASK,
			SAMPLER_NUM,
		};

		UberShaderProgram();
		~UberShaderProgram();

		void init(const std::vector<unsigned char> &baseShader, const char *prepend="", const char *append = "");

		void release();

		typedef Closure<void (IVideoDriver* , ShaderProgram *)> ShaderUniformsCallback;
		void setShaderUniformsCallback(ShaderUniformsCallback cb){_cb = cb;}

		ShaderUniformsCallback	getShaderUniformsCallback() const {return _cb;}
		shader*					getShaderProgram(int flags);


	protected:
		void *_getRestorableObject() {return this;}
		void _restore(Restorable *, void*);

		shader _shaders[SIZE];
		std::vector<unsigned char> _data;
		ShaderUniformsCallback _cb;		

		void releaseShaders();
	};
}