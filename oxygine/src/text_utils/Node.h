#pragma once
#include "oxygine_include.h"
#include "core/Object.h"
#include "math/Color.h"
#include "math/Vector2.h"
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
    class STDRenderer;
    class RenderState;
    class TextRenderer2;
    class IElementRenderer;

    namespace text
    {
        typedef std::vector<Symbol> text_data;


        class DrawContext
        {
        public:
            DrawContext(): renderer(0) {}

            Color color;
            Color primary;
            IElementRenderer* renderer;
        };

        class Node: public PoolObject
        {
        public:
            Node();
            virtual ~Node();

            void appendNode(Node* tn);
            void resize(Aligner& rd);
            void finalPass(Aligner& rd);
            void drawChildren(DrawContext& dc);
            virtual Symbol* getSymbol(int& pos);

            virtual void draw(DrawContext& dc);

            virtual void _resize(Aligner& rd) {}
            virtual void _finalPass(Aligner& rd) {}


            Node* _firstChild;
            Node* _lastChild;
            Node* _nextSibling;
        };

        class TextNode: public Node
        {
        public:
            static void setDefaultMissingSymbol(int);

            TextNode(const char* v);

            text_data _data;
            void _resize(Aligner& rd);
            void _finalPass(Aligner& rd);
            void draw(DrawContext& dc);

            Symbol* getSymbol(int& pos) OVERRIDE;

#ifdef OX_DEBUG
            std::string _text;//only for debug
#endif
        };

        class DivNode: public Node
        {
        public:
            DivNode(const pugi::xml_node& node);
            void _resize(Aligner& rd);
            void draw(DrawContext& dc);

            Color color;
        };

        class BrNode: public Node
        {
        public:
            void _resize(Aligner& rd)
            {
                rd.nextLine();
            }
        };
    }
}
