#include "Aligner.h"
#include "Font.h"
#include <assert.h>
#include "res/ResFont.h"
namespace oxygine
{
    namespace text
    {

        Aligner::Aligner(const TextStyle& Style, const Font* font, float scale, const Vector2& size): width((int)size.x), height((int)size.y), _x(0), _y(0), _lineWidth(0), bounds(0, 0, 0, 0), style(Style), _scale(scale), _font(font)
        {
            _line.reserve(50);
            _lineSkip = _font->getLineHeight() * style.baselineScale + style.linesOffset;
            options = Style.options;
        }

        Aligner::~Aligner()
        {

        }

        float Aligner::_alignX(float rx)
        {
            float tx = 0;
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

        float Aligner::_alignY(float ry)
        {
            float ty = 0;

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
            _y = getLineSkip();
            _lineWidth = 0;

            width = width * _scale;
            height = height * _scale;

            bounds = RectF(_alignX(0), _alignY(0), 0, 0);
        }

        void Aligner::end()
        {
            _alignLine(_line);

            bounds.setY(_alignY(_y));
            bounds.setHeight(_y);
        }

        float Aligner::getLineWidth() const
        {
            return _lineWidth;
        }

        float Aligner::getLineSkip() const
        {
            return _lineSkip;
        }

        void Aligner::_alignLine(line& ln)
        {
            if (!ln.empty())
            {
                //calculate real text width
                float rx = 0;
                for (size_t i = 0; i < ln.size(); ++i)
                {
                    Symbol& s = *ln[i];
                    rx = std::fmaxf(s.x + s.gl.advance_x, rx);
                }

                float tx = _alignX(rx);

                for (size_t i = 0; i < ln.size(); ++i)
                {
                    Symbol& s = *ln[i];
                    s.x += tx;
                }

                _lineWidth = rx;

                bounds.setX(std::fminf(tx, bounds.getX()));
                bounds.setWidth(std::fmaxf(_lineWidth, bounds.getWidth()));
            }
        }

        void Aligner::_nextLine(line& ln)
        {
            _alignLine(ln);
            _y += getLineSkip();

            _lineWidth = 0;

            _x = 0;
        }

        void Aligner::nextLine()
        {
            assert(getStyle().multiline);

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

            float rx = s.x + s.gl.advance_x;


            _lineWidth = std::fmaxf(rx, _lineWidth);

            if (_lineWidth > width && getStyle().multiline && width > 0 && _line.size() > 1)
            {
                size_t lastWordPos = _line.size() - 1;
                for (; lastWordPos > 0; --lastWordPos)
                {
                    if (_line[lastWordPos]->code == ' ' && _line[lastWordPos - 1]->code != ' ')
                        break;
                }

                if (!lastWordPos)
                {
                    if (style.breakLongWords)
                        lastWordPos = _line.size() - 1;
                    else
                        return 0;
                }

                size_t delta = _line.size() - lastWordPos;
                line leftPart;
                leftPart.resize(delta + 1);
                leftPart.assign(_line.begin() + lastWordPos, _line.end());
                _line.resize(lastWordPos);

                nextLine();

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
