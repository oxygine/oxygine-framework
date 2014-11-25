#pragma once
#include <vector>
#include "Restorable.h"
//#include <unordered_map>

namespace oxygine
{
	class ShaderProgram;
	class IVideoDriver;

	class UberShaderProgramBase: public Restorable
	{
	public:
		struct shader
		{
			shader():program(0), flags(0){}
			ShaderProgram *program;
			int flags;
		};		

		UberShaderProgramBase();
		~UberShaderProgramBase();

		void init(const std::vector<unsigned char> &baseShader, const char *prepend="", const char *append = "");

		void release();

		typedef Closure<void (IVideoDriver* , ShaderProgram *)> ShaderUniformsCallback;
		void setShaderUniformsCallback(ShaderUniformsCallback cb){_cb = cb;}

		ShaderUniformsCallback	getShaderUniformsCallback() const {return _cb;}
		virtual shader*			getShaderProgram(int flags) = 0;


	protected:
		void *_getRestorableObject() {return this;}
		void _restore(Restorable *, void*);

		//typedef std::unordered_map<int, shader*> shaders;
		//shaders _shaders;

		//shader* _shaders[SIZE];
		
		std::vector<unsigned char> _data;
		ShaderUniformsCallback _cb;		

		virtual void releaseShaders(){}
	};

	class UberShaderProgram : public UberShaderProgramBase
	{
	public:

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

		shader*					getShaderProgram(int flags);

	protected:
		void releaseShaders();
		shader _shaders[SIZE];
	};
}