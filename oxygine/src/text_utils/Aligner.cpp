#include "Aligner.h"
#include "Font.h"
#include <assert.h>

namespace oxygine
{
    namespace text
    {

        Aligner::Aligner(): width(0), height(0), _x(0), _y(0), _lineWidth(0), bounds(0, 0, 0, 0)
        {
            _line.reserve(50);
        }

        Aligner::~Aligner()
        {

        }

        int Aligner::_alignX(int rx)
        {
            int tx = 0;
            switch (getStyle().hAlign)
            {
                case TextStyle::HALIGN_LEFT:
                case TextStyle::HALIGN_DEFAULT:
                    tx = 0;
                    break;
                case TextStyle::HALIGN_MIDDLE:
                    tx = width / 2 - rx / 2;
                    break;
                case TextStyle::HALIGN_RIGHT:
                    tx = width - rx;
                    break;
            }
            return tx;
        }

        int Aligner::_alignY(int ry)
        {
            int ty = 0;

            switch (getStyle().vAlign)
            {
                case TextStyle::VALIGN_BASELINE:
                    ty = -getLineSkip();
                    break;
                case TextStyle::VALIGN_TOP:
                case TextStyle::VALIGN_DEFAULT:
                    ty = 0;
                    break;
                case TextStyle::VALIGN_MIDDLE:
                    ty = height / 2 - ry / 2;
                    break;
                case TextStyle::VALIGN_BOTTOM:
                    ty = height - ry;
                    break;
            }
            return ty;
        }

        void Aligner::begin()
        {
            _x = 0;
            _y = 0;

            const TextStyle& st = getStyle();
            _scale = st.font->getScale();
            if (st.fontSize2Scale)
                _scale = st.font->getSize() / float(st.fontSize2Scale);

            width = int(width * _scale);
            height = int(height * _scale);

            bounds = Rect(_alignX(0), _alignY(0), 0, 0);
            nextLine();
        }

        void Aligner::end()
        {
            int ry = _y;

            if (getStyle().multiline)
            {
                nextLine();
                _y -=  getLineSkip();
            }
            else
            {
                _alignLine(_line);
            }

            bounds.setY(_alignY(ry));
            bounds.setHeight(ry);
        }

        int Aligner::getLineWidth() const
        {
            return _lineWidth;
        }

        int Aligner::getLineSkip() const
        {
            return getStyle().font->getBaselineDistance() + getStyle().linesOffset;
        }

        void Aligner::_alignLine(line& ln)
        {
            if (!ln.empty())
            {
                //calculate real text width
                int rx = 0;
                for (size_t i = 0; i < ln.size(); ++i)
                {
                    Symbol& s = *ln[i];
                    rx = std::max(s.x + s.gl.sw, rx);
                }

                int tx = _alignX(rx);

                for (size_t i = 0; i < ln.size(); ++i)
                {
                    Symbol& s = *ln[i];
                    s.x += tx;
                }

                _lineWidth = rx;

                bounds.setX(std::min(tx, bounds.getX()));
                bounds.setWidth(std::max(_lineWidth, bounds.getWidth()));
            }
        }

        void Aligner::_nextLine(line& ln)
        {
            _y += getLineSkip();
            _alignLine(ln);


            _lineWidth = 0;

            _x = 0;
        }

        void Aligner::nextLine()
        {
            //assert(multiline == true); commented, becase even if multiline is false - there are breakLine markers, they could be used anyway

            _nextLine(_line);
            _line.clear();
        }

        float Aligner::getScale() const
        {
            return _scale;
        }

        int Aligner::putSymbol(Symbol& s)
        {
            if (_line.empty() && s.code == ' ')
                return 0;

            _line.push_back(&s);

            //optional.. remove?
            if (_line.size() == 1 && s.gl.offset_x < 0)
                _x -= s.gl.offset_x;

            s.x = _x + s.gl.offset_x;
            s.y = _y + s.gl.offset_y;
            _x += s.gl.advance_x + getStyle().kerning;

            int rx = s.x + s.gl.sw;


            _lineWidth = std::max(rx, _lineWidth);

            //
            if (_lineWidth > width && getStyle().multiline && (width > 0) && _line.size() > 1)
            {
                int lastWordPos = (int)_line.size() - 1;
                for (; lastWordPos > 0; --lastWordPos)
                {
                    if (_line[lastWordPos]->code == ' ' && _line[lastWordPos - 1]->code != ' ')
                        break;
                }

                if (!lastWordPos)
                {
                    if (style.breakLongWords)
                        lastWordPos = (int)_line.size() - 1;
                    else
                        return 0;
                }


                int delta = (int)_line.size() - lastWordPos;
                line leftPart;
                leftPart.resize(delta + 1);
                leftPart.assign(_line.begin() + lastWordPos, _line.end());
                _line.resize(lastWordPos);
                nextLine();

                //line = leftPart;

                for (size_t i = 0; i < leftPart.size(); ++i)
                {
                    putSymbol(*leftPart[i]);
                }

                return 0;
            }

            assert(_x > -1000);

            return 0;
        }
    }
}