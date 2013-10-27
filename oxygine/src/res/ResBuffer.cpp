#include "ResBuffer.h"
#include "CreateResourceContext.h"

namespace oxygine
{
	Resource *ResBuffer::create(CreateResourceContext &context)
	{
		ResBuffer *rs = new ResBuffer();
		string file = context.node.attribute("file").value();

		rs->setName(Resource::extractID(context.node, file, ""));
		rs->init((*context.folder + file).c_str());
		setNode(rs, context.node);

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