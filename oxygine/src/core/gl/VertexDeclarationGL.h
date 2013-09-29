#pragma once
#include "oxygine_include.h"
#include "../VertexDeclaration.h"
#include "oxgl.h"
namespace oxygine
{
	class VertexDeclarationGL: public VertexDeclaration
	{
	public:
		void init(bvertex_format);

		struct Element
		{
			char name[16];
			int index;
			bool normalized;
			GLint size;
			GLenum elemType;
			int offset;
		};

		Element elements[8];

	private:
		
	};
}