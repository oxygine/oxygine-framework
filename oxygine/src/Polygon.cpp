#include "Polygon.h"
#include "Serialize.h"
#include "RenderState.h"
#include "core/VertexDeclaration.h"
#include <sstream>

namespace oxygine
{
	Polygon::Polygon() :_verticesSize(0), _verticesData(0), _bformat(0), _own(false)
	{

	}

	Polygon::~Polygon()
	{
		if (_own)
			delete[] _verticesData;
	}

	void Polygon::copyFrom(const Polygon &src, cloneOptions opt)
	{
		Sprite::copyFrom(src, opt);
	}

	void Polygon::serialize(serializedata* data)	
	{
		Sprite::serialize(data);

		pugi::xml_node node = data->node;
		node.set_name("Polygon");
	}

	void Polygon::deserialize(const deserializedata* data)
	{
		Sprite::deserialize(data);
	}

	std::string Polygon::dump(const dumpOptions &options) const
	{
		stringstream stream;
		stream << "{Polygon}\n";

		if (_verticesSize)
		{
			const VertexDeclaration *decl = IVideoDriver::instance->getVertexDeclaration(_bformat);
			stream << "vertices=" << _verticesSize/decl->size << " ";
		}

		stream << Sprite::dump(options);
		return stream.str();
	}
	
	void Polygon::setVertices(const void* data, int size, int bformat, bool own)
	{
		if (_own && data != _verticesData)
		{
			delete[] _verticesData;
		}

		_own = own;
		_bformat = bformat;
		_verticesData = reinterpret_cast<const unsigned char*>(data);
		_verticesSize = size;
	}
	
	void Polygon::doRender(const RenderState &rs)
	{
		if (!_verticesSize)
			return;

		//Sprite
		_vstyle._apply(rs);
		const Diffuse &df = _frame.getDiffuse();
		
		//if (df.base)
		{
			rs.renderer->setDiffuse(df);
			rs.renderer->draw(_verticesData, _verticesSize, _bformat, true);
		}
	}
}