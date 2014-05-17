#include <sstream>

#include "TextActor.h"
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

namespace oxygine
{
	TextActor::TextActor():
		_root(0),
		_textRect(0,0,0,0)
	{
		_flags |= flag_rebuild;
		if (DebugActor::resSystem)
		{
			_style.font = DebugActor::resSystem->getResFont("system")->getFont();
		}
	}

	TextActor::~TextActor()
	{
		delete _root;
		_root = 0;
	}

	void TextActor::copyFrom(const TextActor &src, cloneOptions opt)
	{
		VStyleActor::copyFrom(src, opt);
		_text = src._text;
		_style = src._style;
		_root = 0;

		_flags |= flag_rebuild;
		_textRect = src._textRect;
	}

	bool TextActor::isOn(const Vector2 &localPosition)
	{
		Rect r = getTextRect();
		r.expand(Point(_extendedIsOn, _extendedIsOn), Point(_extendedIsOn, _extendedIsOn));
		return r.pointIn(Point((int)localPosition.x, (int)localPosition.y));
	}

	void TextActor::needRebuild()
	{
		_flags |= flag_rebuild;
	}

	void TextActor::setVAlign(TextStyle::VerticalAlign align)
	{
		_style.vAlign = align;
		needRebuild();
	}

	void TextActor::setMultiline(bool multiline)
	{
		_style.multiline = multiline;
		needRebuild();
	}

	void TextActor::setLinesOffset(int offset)
	{
		_style.linesOffset = offset;
		needRebuild();
	}

	void TextActor::setFontSize2Scale(int scale2size)
	{
		_style.fontSize2Scale = scale2size;
		needRebuild();
	}

	void TextActor::setFont(Font *font)
	{
		_style.font = font;
	}

	void TextActor::setHAlign(TextStyle::HorizontalAlign align)
	{
		_style.hAlign = align;
		needRebuild();
	}

	void TextActor::setStyle(const TextStyle &st)
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

	void TextActor::sizeChanged(const Vector2& size)
	{
		needRebuild();
	}

	void TextActor::setText(const string &str)
	{
		_flags &= ~flag_html;
		if (_text != str)
		{
			_text = str;
			needRebuild();
		}
	}

	void TextActor::setText(const wstring &str)
	{
		setText(ws2utf8(str.c_str()));
	}

	void TextActor::setHtmlText(const string &str)
	{
		_flags |= flag_html;
		if (_text != str)
		{
			_text = str;
			needRebuild();
		}
	}

	void TextActor::setHtmlText(const wstring &str)
	{
		setHtmlText(ws2utf8(str.c_str()));
	}

	const Rect &TextActor::getTextRect()
	{
		getRootNode();
		return _textRect;
	}

	text::Node *TextActor::getRootNode()
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
		case TextStyle::HALIGN_CENTER:
			return "center";
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

	std::string TextActor::dump(const dumpOptions &options) const
	{
		stringstream stream;
		stream << "{TextActor}\n";
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

		Rect r = const_cast<TextActor*>(this)->getTextRect();
		stream << " textRect=("<< r.pos.x << ", "<< r.pos.y << ", "<< r.size.x << ", "<< r.size.y << ")";
		
		stream << "\n" << Actor::dump(options);

		return stream.str();
		
	}

//#define  SD_FONT

	void TextActor::doRender(RenderState const& rs)
	{
		text::Node *root = getRootNode();
		if (!root)
			return;

		VisualStyle vs = _vstyle;
		vs.setColor(getColor() * _style.color);
		vs._apply(rs);
		
		text::DrawContext dc;	
		dc.rs = &rs;
		dc.renderer = rs.renderer;

#ifdef SD_FONT
		dc.renderer->drawBatch();
		VideoDriverGLES20 *driver = (VideoDriverGLES20 *)dc.renderer->getDriver();
		driver->setProgramSDF();		
#endif
		root->draw(dc);

#ifdef SD_FONT
		dc.renderer->drawBatch();
		driver->setProgramMain();
#endif		
	}

	void TextActor::serialize(serializedata* data)
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
		node.set_name("TextActor");
	}

	void TextActor::deserialize(const deserializedata* data)
	{
		VStyleActor::deserialize(data);
		pugi::xml_node node = data->node;

		TextStyle def;
		
		_style.vAlign = (TextStyle::VerticalAlign)node.attribute("valign").as_int(def.vAlign);
		_style.hAlign = (TextStyle::HorizontalAlign)node.attribute("halign").as_int(def.hAlign);
		_style.multiline = node.attribute("multiline").as_bool(def.multiline);
		_style.fontSize2Scale = node.attribute("fontsize2scale").as_int(def.fontSize2Scale);
		needRebuild();
		setText(node.attribute("text").as_string());
	}
}