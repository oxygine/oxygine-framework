#include "TextBuilder.h"
#include "Node.h"
#include "utils/stringUtils.h"
#include "pugixml/pugixml.hpp"

namespace oxygine
{
	namespace text
	{
		TextBuilder::TextBuilder()
		{

		}

		TextBuilder::~TextBuilder()
		{

		}

		text::Node *create(const pugi::xml_node &node)
		{
			const char *v = node.value();
			text::Node *tn = 0;
		
		
			if (!strcmp(node.name(), "div"))
			{
				tn = new text::DivNode(node);
			}
			else
				if (!strcmp(node.name(), "br"))
				{
					tn = new text::BrNode();
				}
				else
					if (v && v[0])
					{
						tn = new text::TextNode(v);
					}
					else
						tn = new text::Node;

			pugi::xml_node child = node.first_child();
			while(child)
			{
				text::Node *tnchild = create(child);
				tn->appendNode(tnchild);
				child = child.next_sibling();
			}

			return tn;
		}

		text::Node *TextBuilder::parse(const std::string &st)
		{
			std::string str = "<r>" + st + "</r>";
			pugi::xml_document doc;		

			int flags = pugi::parse_default;
			pugi::xml_parse_result res = doc.load_buffer(str.c_str(), str.size(), flags);
			if (!res)
			{
				OX_ASSERT(!"can't parse tagged text");
			}

			return create(doc.first_child());
		}
	}
}