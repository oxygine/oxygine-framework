#pragma once
#include "oxygine_include.h"
#include "Sprite.h"

namespace oxygine
{
	class ResAnim;

	DECLARE_SMART(Polygon, spPolygon);
	class Polygon : public Sprite
	{
	public:		
		DECLARE_COPYCLONE_NEW(Polygon);
		typedef std::vector<vertexPCT2> vertices;

		Polygon();
		~Polygon();

		/**Sets vertices for drawing*/
		//void setVertices(const vertices &vert);
		/**Sets vertices for drawing. You should control data and delete buffer youself.*/
		//void setVertices(const vertexPCT2* data, int num);

		void setVertices(const void* data, int size, int bformat, bool own);

		/**sets vertices for drawing by using std::swap with argument.*/
		//void setVerticesAndSwap(vertices &vert);

		void serialize(serializedata* data);
		void deserialize(const deserializedata* data);

		std::string dump(const dumpOptions &) const;

	protected:		
		void doRender(const RenderState &rs);

		const VertexDeclaration *_vdecl;
		bool _own;
		const unsigned char *_verticesData;
		int _verticesSize;
	};
}