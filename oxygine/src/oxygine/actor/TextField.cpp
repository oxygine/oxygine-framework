#include "TextField.h"
#include "DebugActor.h"
#include "../Font.h"
#include "../RenderDelegate.h"
#include "../RenderState.h"
#include "../Serialize.h"
#include "../core/gl/VideoDriverGLES20.h"
#include "../res/Resources.h"
#include "../text_utils/Node.h"
#include "../text_utils/TextBuilder.h"
#include "../utils/stringUtils.h"
#include <sstream>

namespace oxygine
{
    static ResFont* _defaultFont = 0;
    void TextField::setDefaultFont(ResFont* f)
    {
        _defaultFont = f;
    }

    ResFont* TextField::getDefaultFont()
    {
        return _defaultFont;
    }

    TextField::TextField():
        _root(0),
        _textRect(0, 0, 0, 0),
        _rtscale(1.0f)
    {
        _flags |= flag_rebuild;
        _style.font = _defaultFont;
    }

    TextField::~TextField()
    {
        delete _root;
        _root = 0;
    }

    void TextField::copyFrom(const TextField& src, cloneOptions opt)
    {
        inherited::copyFrom(src, opt);
        _text = src._text;
        _style = src._style;
        _root = 0;
        _rtscale = 1.0f;

        _flags |= flag_rebuild;
        _textRect = src._textRect;
    }

    bool TextField::isOn(const Vector2& localPosition, float localScale)
    {
        Rect r = getTextRect(localScale);
        r.expand(Point(_extendedIsOn, _extendedIsOn), Point(_extendedIsOn, _extendedIsOn));
        return r.pointIn(Point((int)localPosition.x, (int)localPosition.y));
    }

    void TextField::needRebuild()
    {
        _flags |= flag_rebuild;
    }

    void TextField::setVAlign(TextStyle::VerticalAlign align)
    {
        _style.vAlign = align;
        needRebuild();
    }

    void TextField::setMultiline(bool multiline)
    {
        _style.multiline = multiline;
        needRebuild();
    }

    void TextField::setBreakLongWords(bool val)
    {
        _style.breakLongWords = val;
        needRebuild();
    }

    void TextField::setLinesOffset(int offset)
    {
        _style.linesOffset = offset;
        needRebuild();
    }

    void TextField::setBaselineScale(float s)
    {
        _style.baselineScale = s;
        needRebuild();
    }

    void TextField::setKerning(int kerning)
    {
        _style.kerning = kerning;
        needRebuild();
    }

    void TextField::setFontSize(int size)
    {
        _style.fontSize = size;
        needRebuild();
    }

    void TextField::setStyleColor(const Color& color)
    {
        _style.color = color;
        needRebuild();
    }

    void TextField::setOptions(size_t opt)
    {
        _style.options = opt;
        needRebuild();
    }

    const ResFont* TextField::getFont() const
    {
        return _style.font;
    }

    void TextField::setFont(const ResFont* font)
    {
        _style.font = font;
        if (!_style.font)
            _style.font = _defaultFont;
        needRebuild();
    }

    void TextField::setHAlign(TextStyle::HorizontalAlign align)
    {
        _style.hAlign = align;
        needRebuild();
    }

    void TextField::setAlign(TextStyle::VerticalAlign vAlign, TextStyle::HorizontalAlign hAlign)
    {
        _style.vAlign = vAlign;
        _style.hAlign = hAlign;
        needRebuild();
    }

    void TextField::setStyle(const TextStyle& st)
    {
        TextStyle::HorizontalAlign halign = _style.hAlign;
        TextStyle::VerticalAlign valign = _style.vAlign;
        int size = _style.fontSize;
        _style = st;

        if (st.hAlign == TextStyle::HALIGN_DEFAULT)
            _style.hAlign = halign;

        if (st.vAlign == TextStyle::VALIGN_DEFAULT)
            _style.vAlign = valign;

        if (st.fontSize == 0)
            _style.fontSize = size;

        if (!_style.font)
            _style.font = _defaultFont;

        needRebuild();
    }

    void TextField::sizeChanged(const Vector2& size)
    {
        needRebuild();
    }

    void TextField::matChanged()
    {
        log::warning("not optimal");
        if (_flags & flag_rebuild)
            return;

        if (!_root)
            return;

        _root->updateMaterial(*_mat.get());
    }

    void TextField::setText(const std::string& str)
    {
        _flags &= ~flag_html;
        if (_text != str)
        {
            _text = str;
            needRebuild();
        }
    }

    void TextField::setText(const std::wstring& str)
    {
        setText(ws2utf8(str.c_str()));
    }

    void TextField::setHtmlText(const std::string& str)
    {
        _flags |= flag_html;
        if (_text != str)
        {
            _text = str;
            needRebuild();
        }
    }

    void TextField::setHtmlText(const std::wstring& str)
    {
        setHtmlText(ws2utf8(str.c_str()));
    }

    int            TextField::getFontSize() const
    {
        return _style.fontSize;
    }

    int TextField::getLinesOffset() const
    {
        return _style.linesOffset;
    }

    TextStyle::VerticalAlign    TextField::getVAlign() const
    {
        return _style.vAlign;
    }

    TextStyle::HorizontalAlign  TextField::getHAlign() const
    {
        return _style.hAlign;
    }

    bool TextField::getMultiline() const
    {
        return _style.multiline;
    }

    bool TextField::getBreakLongWords() const
    {
        return _style.breakLongWords;
    }

    int TextField::getKerning() const
    {
        return _style.kerning;
    }

    const oxygine::Color& TextField::getStyleColor() const
    {
        return _style.color;
    }

    float TextField::getBaselineScale() const
    {
        return _style.baselineScale;
    }

    size_t TextField::getOptions() const
    {
        return _style.options;
    }

    text::Symbol* TextField::getSymbolAt(int pos) const
    {
        return const_cast<TextField*>(this)->getRootNode(_rtscale)->getSymbol(pos);
    }

    const Rect& TextField::getTextRect(float localScale) const
    {
        const_cast<TextField*>(this)->getRootNode(localScale);
        return _textRect;
    }

    bool TextField::getBounds(RectF& r) const
    {
        r = getTextRect().cast<RectF>();
        return true;
    }


    text::Node* TextField::getRootNode(float globalScale)
    {
        if (!_style.font)
            return _root;


        float scale = 1.0f;
        const Font* font = _style.font->getClosestFont(globalScale, _style.fontSize, scale);

        if ((_flags & flag_rebuild || _rtscale != scale) && font)
        {
            _rtscale = scale;
            //_realFontSize = fontSize;
            delete _root;

            _flags &= ~flag_rebuild;

            if (_flags & flag_html)
            {
                text::TextBuilder b;
                _root = b.parse(_text);
            }
            else
            {
                _root = new text::TextNode(_text.c_str());
            }

            text::Aligner rd(_style, _mat, font, scale, getSize());
            rd.begin();
            _root->resize(rd);
            rd.end();

            _root->finalPass(rd);
            rd.bounds = (rd.bounds.cast<RectF>() / rd.getScale()).cast<Rect>();

            _textRect = rd.bounds;

            Event ev(EVENT_REBUILD);
            dispatchEvent(&ev);
        }

        return _root;
    }

    const char* get_valign(TextStyle::VerticalAlign v)
    {
        switch (v)
        {

            case TextStyle::VALIGN_BASELINE:
                return "baseline";
            case TextStyle::VALIGN_TOP:
                return "top";
            case TextStyle::VALIGN_BOTTOM:
                return "bottom";
            case TextStyle::VALIGN_MIDDLE:
                return "middle";
            default:
                break;
        }
        return "unknown";
    }

    const char* get_halign(TextStyle::HorizontalAlign v)
    {
        switch (v)
        {
            case TextStyle::HALIGN_DEFAULT:
                return "default";
            case TextStyle::HALIGN_LEFT:
                return "left";
            case TextStyle::HALIGN_RIGHT:
                return "right";
            case TextStyle::HALIGN_MIDDLE:
                return "middle";
        }
        return "unknown";
    }

    std::string dumpStyle(const TextStyle& s, bool onlydiff)
    {
        TextStyle def;
        std::stringstream stream;
        if (!onlydiff || def.hAlign != s.hAlign)
            stream << "hAlign=" << get_halign(s.hAlign);
        if (!onlydiff || def.vAlign != s.vAlign)
            stream << " vAlign=" << get_valign(s.vAlign);
        if (!onlydiff || def.multiline != s.multiline)
            stream << " " << (s.multiline ? "multiline" : "singleline");
        if (!onlydiff || def.breakLongWords != s.breakLongWords)
            stream << " " << (s.breakLongWords ? "breakLongWords=1" : "breakLongWords=0");
        if (!onlydiff || def.kerning != s.kerning)
            stream << " kerning=" << s.kerning;
        if (!onlydiff || def.baselineScale != s.baselineScale)
            stream << " baselineScale=" << s.baselineScale;
        if (!onlydiff || def.linesOffset != s.linesOffset)
            stream << " linesOffset=" << s.linesOffset;
        if (!onlydiff || def.fontSize != s.fontSize)
            stream << " fontSize=" << s.fontSize;
        if (s.font)
        {
            stream << " font='" << s.font->getName() << "'";
        }

        return stream.str();
    }

    std::string TextField::dump(const dumpOptions& options) const
    {
        std::stringstream stream;
        stream << "{TextField}\n";
        stream << _vstyle.dump();
        std::string text = _text;
        if (text.size() > 15)
        {
            text.resize(15);
            text += "...";
        }
        stream << " text=<div c='2b1a94'>'<![CDATA[" << text << "']]></div>";

        std::string st = dumpStyle(_style, true);
        if (st.size())
            stream << " textStyle={" << st << "}";
        if (_flags & flag_html)
        {
            stream << " htmlMode";
        }

        Rect r = const_cast<TextField*>(this)->getTextRect();
        stream << " textRect=(" << r.pos.x << ", " << r.pos.y << ", " << r.size.x << ", " << r.size.y << ")";

        stream << "\n" << Actor::dump(options);

        return stream.str();

    }

    void TextField::doRender(RenderState const& rs)
    {
        _rdelegate->doRender(this, rs);
    }

    void TextField::serialize(serializedata* data)
    {
        inherited::serialize(data);
        pugi::xml_node node = data->node;

        TextStyle def;

        if (!_text.empty())
            node.append_attribute("text").set_value(_text.c_str());
        setAttr(node, "fontsize2scale", _style.fontSize, def.fontSize);
        setAttr(node, "linesOffset", _style.linesOffset, def.linesOffset);
        setAttr(node, "kerning", _style.kerning, def.kerning);
        setAttr(node, "valign", _style.vAlign, def.vAlign);
        setAttr(node, "halign", _style.hAlign, def.hAlign);
        setAttr(node, "multiline", _style.multiline, def.multiline);
        setAttr(node, "baselineScale", _style.baselineScale, def.baselineScale);
        setAttr(node, "breakLongWords", _style.breakLongWords, def.breakLongWords);
        if (_style.font)
            node.append_attribute("font").set_value(_style.font->getName().c_str());
        node.set_name("TextField");
    }

    void TextField::deserialize(const deserializedata* data)
    {
        inherited::deserialize(data);
        pugi::xml_node node = data->node;

        TextStyle def;

        _style.vAlign = (TextStyle::VerticalAlign)node.attribute("valign").as_int(def.vAlign);
        _style.hAlign = (TextStyle::HorizontalAlign)node.attribute("halign").as_int(def.hAlign);
        _style.multiline = node.attribute("multiline").as_bool(def.multiline);
        _style.breakLongWords = node.attribute("breakLongWords").as_bool(def.breakLongWords);
        _style.fontSize = node.attribute("fontsize2scale").as_int(def.fontSize);
        _style.linesOffset = node.attribute("linesOffset").as_int(def.linesOffset);
        _style.kerning = node.attribute("kerning").as_int(def.kerning);
        _style.baselineScale = node.attribute("baselineScale").as_float(def.baselineScale);
        const char* fnt = node.attribute("font").as_string(0);
        if (fnt && *fnt)
        {
            ResFont* font = data->factory->getResFont(fnt);
            if (font)
                _style.font = font;
        }

        needRebuild();
        setText(node.attribute("text").as_string());
    }
}
