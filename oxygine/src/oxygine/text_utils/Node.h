#pragma once
#include "../oxygine-include.h"
#include "../core/Object.h"
#include "../math/Color.h"
#include "../math/Vector2.h"
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
            virtual void resize(Aligner& rd);
            void finalPass(Aligner& rd);

            void drawChildren(DrawContext& dc);
            void resizeChildren(Aligner& rd);
            virtual Symbol* getSymbol(int& pos);

            virtual void draw(DrawContext& dc);

            virtual void xresize(Aligner& rd) {}
            virtual void xfinalPass(Aligner& rd) {}

            void updateMaterial(const STDMaterialX& mat);
            virtual void xupdateMaterial(const STDMaterialX& mat) {}


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
            void xresize(Aligner& rd) override;
            void xfinalPass(Aligner& rd) override;
            void draw(DrawContext& dc) override;
            void xupdateMaterial(const STDMaterialX& mat) override;

            Symbol* getSymbol(int& pos) override;

#ifdef OX_DEBUG
            std::string _text;//only for debug
#endif
        };

        class DivNode: public Node
        {
        public:
            DivNode(const pugi::xml_node& node);

            void resize(Aligner& rd) override;
            void draw(DrawContext& dc) override;

            Color color;
            unsigned int options;
        };

        class BrNode: public Node
        {
        public:
            void xresize(Aligner& rd)
            {
                rd.nextLine();
            }
        };
    }
}
