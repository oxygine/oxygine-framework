#include "Node.h"
#include "Font.h"
#include "core/Renderer.h"
#include "pugixml/pugixml.hpp"
#include "utils/stringUtils.h"
#include "RenderState.h"
#include "AnimationFrame.h"

namespace oxygine
{
	namespace text
	{
		Node::Node():_firstChild(0), _lastChild(0), _nextSibling(0)
		{

		}

		Node::~Node()
		{
			Node *child = _firstChild;
			while (child)
			{
				Node *next = child->_nextSibling;
				delete child;

				child = next;
			}
		}

		void Node::appendNode(Node *tn)
		{
			if (!_firstChild)
				_firstChild = tn;
			else
				_lastChild->_nextSibling = tn;

			_lastChild = tn;
		}



		void Node::resize(Aligner &rd)
		{
			_resize(rd);

			Node *child = _firstChild;
			while (child)
			{
				child->resize(rd);
				child = child->_nextSibling;
			}
		}

		void Node::finalPass(Aligner &rd)
		{
			float scaleFactor = rd.getScaleFactor();

			int offsetY = rd.bounds.pos.y;

			_finalPass(rd);

			Node *child = _firstChild;
			while (child)
			{			
				child->finalPass(rd);
				child = child->_nextSibling;
			}
		}

		void Node::drawChildren(DrawContext &dc)
		{
			Node *child = _firstChild;
			while (child)
			{			
				child->draw(dc);
				child = child->_nextSibling;
			}
		}

		void Node::draw(DrawContext &dc)
		{
			drawChildren(dc);
		}


		
		/*
		bool test()
		{
			string utf = ws2utf8(L"A");
			wstring ws = utf8tows(utf.c_str());			

			const char *bf = (char*)ws.c_str();

			const char *utfstr = utf.c_str();
			int code = 0;
			while (getNextSingleCode(code, utfstr))
			{
				int q = 0;
			}

			
			return true;
		}

		bool b = test();
		*/

		TextNode::TextNode(const char *v)
		{
#ifdef OX_DEBUG
			_text = v;
#endif

			const char *utfstr = v;
			int code = 0;
			utfstr = getNextCode(code, utfstr);
			while (code)
			{
				Symbol s;
				s.code = code;
				_data.push_back(s);

				utfstr = getNextCode(code, utfstr);
			}
		}

		void TextNode::draw(DrawContext &dc)
		{
			for (size_t i = 0; i < _data.size(); ++i)
			{
				const Symbol &s = _data[i];
				if (!s.gl)
					continue;
				Diffuse df;
				df.base = s.gl->texture;
				dc.renderer->setDiffuse(df);
				dc.renderer->draw(s.gl->src, s.destRect);
			}

			drawChildren(dc);
		}

		int _defMissing = '?';
		void TextNode::setDefaultMissingSymbol(int v)
		{
			_defMissing = v;
		}

		void TextNode::_resize(Aligner &rd)
		{
			if (!_data.empty())
			{
				int i = 0;
				Font *font = rd.getStyle().font;

				while (i != (int)_data.size())
				{
					Symbol &s = _data[i];
					//wchar_t c = s.c;
					s.gl = font->getGlyph(s.code);
					if (s.gl)
						i += rd.putSymbol(s);
					else
					{
						if (s.code == '\n')
							rd.nextLine();
						else
						{
							s.gl = font->getGlyph(_defMissing);
							i += rd.putSymbol(s);
						}
					}
					++i;
					if (i < 0)
						i = 0;
				}
			}
		}

		float mlt(int x, float sf)
		{
			//return (x + 0.01f) / sf;
			return x / sf;
		}

		void TextNode::_finalPass(Aligner &rd)
		{
			float scaleFactor = rd.getScaleFactor();

			int offsetY = rd.bounds.pos.y;

			for (size_t i = 0; i < _data.size(); ++i)
			{
				Symbol &s = _data[i];
				s.y += offsetY;

				if (s.gl)
					s.destRect = RectF(mlt(s.x, scaleFactor), mlt(s.y, scaleFactor), mlt(s.gl->sw, scaleFactor), mlt(s.gl->sh, scaleFactor));
				else
					s.destRect = RectF(0, 0, 0, 0);

				//s.destRect.size = Vector2(4.26666666f, 7.46666666f);				
			}
		}

		void DivNode::_resize(Aligner &rd)
		{

		}

		void DivNode::draw(DrawContext &dc)
		{
			Color prevColor = dc.renderer->getPrimaryColor();
			
			Color blendedColor = color;
			blendedColor.a = (blendedColor.a * dc.rs->alpha) / 255;

			dc.renderer->setPrimaryColor(blendedColor);
			drawChildren(dc);						
			dc.renderer->setPrimaryColor(prevColor);
		}

		DivNode::DivNode(const pugi::xml_node &node)
		{
			pugi::xml_attribute attr = node.first_attribute();
			while(attr)
			{
				if (!strcmp(attr.name(), "c") ||
					!strcmp(attr.name(), "color"))
				{
					color = hex2color(attr.value());
				}
				attr = attr.next_attribute();
			}
		}
	}
}