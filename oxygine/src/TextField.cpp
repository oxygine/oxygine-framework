#include <sstream>

#include "TextField.h"
#include "Font.h"
#include "core/Renderer.h"
#include "core/Texture.h"
#include "DebugActor.h"
#include "res/Resources.h"
#include "RenderState.h"

#include "utils/stringUtils.h"
#include "core/gl/VideoDriverGLES20.h"
#include "text_utils/TextBuilder.h"
#include "text_utils/Node.h"
#include "Serialize.h"

#include "STDRenderer.h"

namespace oxygine
{
	TextField::TextField():
		_root(0),
		_textRect(0,0,0,0)
	{
		_flags |= flag_rebuild;
		_style.font = NULL;

		if (DebugActor::resSystem)
		{
			if (ResFont *fnt = DebugActor::resSystem->getResFont("system")) {
				_style.font = fnt->getFont();
			}
		}
	}

	TextField::~TextField()
	{
		delete _root;
		_root = 0;
	}

	void TextField::copyFrom(const TextField &src, cloneOptions opt)
	{
		VStyleActor::copyFrom(src, opt);
		_text = src._text;
		_style = src._style;
		_root = 0;

		_flags |= flag_rebuild;
		_textRect = src._textRect;
	}

	bool TextField::isOn(const Vector2 &localPosition)
	{
		Rect r = getTextRect();
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

	void TextField::setFontSize2Scale(int scale2size)
	{
		_style.fontSize2Scale = scale2size;
		needRebuild();
	}

	void TextField::setFont(Font *font)
	{
		_style.font = font;
	}

	void TextField::setHAlign(TextStyle::HorizontalAlign align)
	{
		_style.hAlign = align;
		needRebuild();
	}

	void TextField::setStyle(const TextStyle &st)
	{
		TextStyle::HorizontalAlign halign = _style.hAlign;
		TextStyle::VerticalAlign valign = _style.vAlign;
		_style = st;
		
		if (st.hAlign == TextStyle::HALIGN_DEFAULT)
			_style.hAlign = halign;

		if (st.vAlign == TextStyle::VALIGN_DEFAULT)
			_style.vAlign = valign;

		needRebuild();
	}

	void TextField::sizeChanged(const Vector2& size)
	{
		needRebuild();
	}

	void TextField::setText(const string &str)
	{
		_flags &= ~flag_html;
		if (_text != str)
		{
			_text = str;
			needRebuild();
		}
	}

	void TextField::setText(const wstring &str)
	{
		setText(ws2utf8(str.c_str()));
	}

	void TextField::setHtmlText(const string &str)
	{
		_flags |= flag_html;
		if (_text != str)
		{
			_text = str;
			needRebuild();
		}
	}

	void TextField::setHtmlText(const wstring &str)
	{
		setHtmlText(ws2utf8(str.c_str()));
	}

	int TextField::getFontSize2Scale() const
	{
		return _style.fontSize2Scale;
	}

	TextStyle::VerticalAlign	TextField::getVAlign() const
	{
		return _style.vAlign;
	}

	TextStyle::HorizontalAlign	TextField::getHAlign() const
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

	const Rect &TextField::getTextRect()
	{
		getStageNode();
		return _textRect;
	}

	text::Node *TextField::getStageNode()
	{
		if ((_flags & flag_rebuild) && _style.font)
		{
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

			text::Aligner rd;

			rd.width = (int)getWidth();
			rd.height = (int)getHeight();
			rd.style = _style;
			rd.begin();
			_root->resize(rd);
			rd.end();

			_root->finalPass(rd);
			rd.bounds = (rd.bounds.cast<RectF>() / rd.getScaleFactor()).cast<Rect>();

			_textRect = rd.bounds;
		}

		return _root;
	}

	const char *get_valign(TextStyle::VerticalAlign v)
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
		}
		return "unknown";
	}

	const char * get_halign(TextStyle::HorizontalAlign v)
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

	std::string dumpStyle(const TextStyle &s, bool onlydiff)
	{
		TextStyle def;
		stringstream stream;
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
		if (!onlydiff || def.linesOffset != s.linesOffset)
			stream << " linesOffset=" << s.linesOffset;
		if (!onlydiff || def.fontSize2Scale != s.fontSize2Scale)
			stream << " scale2size=" << s.fontSize2Scale;
		if (s.font)
		{
			stream << " font='" << s.font->getName() << "'";
		}

		return stream.str();
	}

	std::string TextField::dump(const dumpOptions &options) const
	{
		stringstream stream;
		stream << "{TextField}\n";
		stream << _vstyle.dump();
		string text = _text;
		if (text.size() > 15)
		{
			text.resize(15);
			text += "...";
		}
		stream << " text=<div c='2b1a94'>'<![CDATA[" << text << "']]></div>";

		string st = dumpStyle(_style, true);
		if (st.size())
			stream << " textStyle={" << st << "}"; 
		if (_flags & flag_html)
		{
			stream << " htmlMode";
		}

		Rect r = const_cast<TextField*>(this)->getTextRect();
		stream << " textRect=("<< r.pos.x << ", "<< r.pos.y << ", "<< r.size.x << ", "<< r.size.y << ")";
		
		stream << "\n" << Actor::dump(options);

		return stream.str();
		
	}

//#define  SD_FONT

	void TextField::doRender(RenderState const& rs)
	{
		text::Node *root = getStageNode();
		if (!root)
			return;

		if (_text == "Tree Inspector")
			int q = 0;

		VisualStyle vs = _vstyle;
		vs._apply(rs);
	
		
		
		//TextRenderer
		text::DrawContext dc;	
		Color color = _vstyle.getColor();
		color.a = (color.a * rs.alpha) / 255;
		dc.primary = color.premultiplied();
		dc.color = _style.color * dc.primary;

		dc.rs = &rs;

		TextRenderer2 tr(safeCast<STDRenderer*>(rs.renderer));
		dc.renderer = &tr;

		root->draw(dc);
	}

	void TextField::serialize(serializedata* data)
	{
		VStyleActor::serialize(data);
		pugi::xml_node node = data->node;

		TextStyle def;
				
		if (!_text.empty())
			node.append_attribute("text").set_value(_text.c_str());
		setAttr(node, "fontsize2scale", _style.fontSize2Scale, def.fontSize2Scale);		
		setAttr(node, "valign", _style.vAlign, def.vAlign);
		setAttr(node, "halign", _style.hAlign, def.hAlign);
		setAttr(node, "multiline", _style.multiline, def.multiline);
		setAttr(node, "breakLongWords", _style.breakLongWords, def.breakLongWords);
		node.set_name("TextField");
	}

	void TextField::deserialize(const deserializedata* data)
	{
		VStyleActor::deserialize(data);
		pugi::xml_node node = data->node;

		TextStyle def;
		
		_style.vAlign = (TextStyle::VerticalAlign)node.attribute("valign").as_int(def.vAlign);
		_style.hAlign = (TextStyle::HorizontalAlign)node.attribute("halign").as_int(def.hAlign);
		_style.multiline = node.attribute("multiline").as_bool(def.multiline);
		_style.breakLongWords = node.attribute("breakLongWords").as_bool(def.breakLongWords);
		_style.fontSize2Scale = node.attribute("fontsize2scale").as_int(def.fontSize2Scale);
		needRebuild();
		setText(node.attribute("text").as_string());
	}
}
