#pragma once
#include "oxygine_include.h"
#include "core/oxygine.h"
#include "core/Object.h"
#include "math/Color.h"
#include "math/vector2.h"
#include "Aligner.h"
#include <string>
#include <vector>

namespace pugi
{
	class xml_node;
}

namespace oxygine
{
	class RenderState;
	struct glyph;

	class Aligner;
	class Renderer;
	class RenderState;
	
	namespace text
	{
		typedef vector<Symbol> text_data;

		class DrawContext
		{
		public:
			DrawContext():rs(0), renderer(0){}

			const RenderState *rs;
			Renderer *renderer;
		};

		class Node: public PoolObject
		{
		public:
			Node();
			virtual ~Node();

			void appendNode(Node *tn);
			void resize(Aligner &rd);
			void finalPass(Aligner &rd);
			void drawChildren(DrawContext &dc);
			virtual void draw(DrawContext &dc);

			virtual void _resize(Aligner &rd){}
			virtual void _finalPass(Aligner &rd){}


			Node *_firstChild;
			Node *_lastChild;
			Node *_nextSibling;
		};

		class TextNode: public Node
		{
		public:
			TextNode(const char *v);

			text_data _data;
			void _resize(Aligner &rd);
			void _finalPass(Aligner &rd);
			void draw(DrawContext &dc);

#ifdef OX_DEBUG
			string _text;//only for debug
#endif
		};

		class DivNode: public Node
		{
		public:
			DivNode(const pugi::xml_node &node);
			void _resize(Aligner &rd);
			void draw(DrawContext &dc);

			Color color;
		};

		class BrNode: public Node
		{
		public:
			void _resize(Aligner &rd)
			{
				rd.nextLine();
			}
		};
	}
}