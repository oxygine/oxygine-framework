#include "ResBuffer.h"
#include "CreateResourceContext.h"

namespace oxygine
{
	Resource *ResBuffer::create(CreateResourceContext &context)
	{
		ResBuffer *rs = new ResBuffer();
		pugi::xml_node node = context.walker.getNode();
		string file = node.attribute("file").value();

		rs->setName(Resource::extractID(node, file, ""));
		rs->init(context.walker.getPath("file").c_str());
		setNode(rs, node);

		return rs;
	}


	ResBuffer::ResBuffer()
	{

	}

	ResBuffer::~ResBuffer()
	{

	}

	void ResBuffer::init(const char *file)
	{
		_path = file;
	}

	void ResBuffer::_load(LoadResourcesContext *)
	{
		file::read(_path.c_str(), _buffer);
	}

	void ResBuffer::_unload()
	{
		_buffer.data.clear();
	}
}