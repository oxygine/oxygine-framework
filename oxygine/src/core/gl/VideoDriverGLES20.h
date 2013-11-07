#pragma once
#include "oxygine_include.h"
#include "VideoDriverGL.h"
#include "core/Restorable.h"
#include "../ShaderProgram.h"
#include "../file.h"

namespace oxygine
{
	class VideoDriverGLES20: public VideoDriverGL
	{
	public:
		VideoDriverGLES20();
		~VideoDriverGLES20();

		void reset();
		void restore();

		spNativeTexture createTexture();
		//const file::buffer& getShaderBody() const {return _shaderBody;}
		bool isReady() const;

		void begin(const Rect &viewport, const Color *clearColor);

		void draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl, const void *verticesData, unsigned int verticesDataSize);
		void draw(PRIMITIVE_TYPE pt, const VertexDeclaration *decl, const void *verticesData, unsigned int verticesDataSize, const void *indicesData, unsigned int indicesDataSize, bool indicesShortType);
		void setDefaultSettings();		

		void setShaderProgram(ShaderProgram*);
		void setTexture(int sampler, spNativeTexture);

	protected:		
		void updateConstants();
	};
}