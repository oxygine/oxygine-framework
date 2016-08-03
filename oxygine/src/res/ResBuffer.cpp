#include "ResBuffer.h"
#include "Resources.h"
#include "CreateResourceContext.h"

namespace oxygine
{
    Resource* ResBuffer::create(CreateResourceContext& context)
    {
        ResBuffer* rs = new ResBuffer();
        pugi::xml_node node = context.walker.getNode();
        std::string file = node.attribute("file").value();

        rs->setName(_Resource::extractID(node, file, ""));
        rs->init(context.walker.getPath("file").c_str());
        setNode(rs, node);

        context.resources->add(rs);

        return rs;
    }


    ResBuffer::ResBuffer()
    {

    }

    ResBuffer::~ResBuffer()
    {

    }

    void ResBuffer::init(const char* file)
    {
        _path = file;
    }

    void ResBuffer::_load(LoadResourcesContext*)
    {
        file::read(_path, _buffer);
    }

    void ResBuffer::_unload()
    {
        _buffer.data.clear();
    }
}
