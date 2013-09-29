#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "core/Renderer.h"
#include "TextStyle.h"
#include "VisualStyle.h"
namespace oxygine
{
	DECLARE_SMART(TextActor, spTextActor);

	namespace text
	{
		class Node;
	}
	

	class TextActor : public Actor
	{
	public:
		DECLARE_COPYCLONE(TextActor);
		TextActor();
		~TextActor();
		
		const TextStyle&	getStyle() const {return _style;}
		const Rect&			getTextRect();
		const string&		getText() const {return _text;}		
		const Color&		getColor() const {return _vstyle.getColor();}

		void setColor(const Color &color){_vstyle.setColor(color);}
		void setBlendMode(blend_mode m){_vstyle.setBlendMode(m);}

		/**Overwrites TextStyle Vertical align*/
		void setVAlign(TextStyle::VerticalAlign align);
		/**Overwrites TextStyle Horizontal align*/
		void setHAlign(TextStyle::HorizontalAlign align);
		/**Overwrites TextStyle multiline*/
		void setMultiline(bool multiline);
		/**Overwrites TextStyle linesOffset*/
		void setLinesOffset(int offset);
		/**Overwrites TextStyle scale2Size.*/
		void setFontSize2Scale(int scale2size);
		/**Overwrites TextStyle font.*/
		void setFont(Font *rs);

		void setStyle(const TextStyle &st);
		/**Changes text utf8*/
		void setText(const string &str);

		void setText(const wstring &str);
		/**Changes text. 
		Supports html style tags:  <br/> - break line,  <div color='FFFFFF'/>colored text </div>, <div c='FFFFFF'/>colored text</div> 
		example: 
		This is <div c='ff0000'/> colored text <div/>supports  <div c='00ff00AA'/>nested tags</div><br/> with broken line.
		*/
		void setHtmlText(const string &str);
		void setHtmlText(const wstring &str);

		bool isOn(const Vector2 &localPosition);		

		
		std::string dump(const dumpOptions &options) const;
		void doRender( RenderState const& parentRenderState ); 

		typedef GetSet<Color, const Color &, TextActor, &TextActor::getColor, &TextActor::setColor>	TweenColor;

	protected:
		string  _text;
		bool	_html;

		TextStyle _style;
		VisualStyle _vstyle;

		text::Node *_root;
		bool _rebuild;
		Rect _textRect;

		text::Node *getRootNode();
		void needRebuild();
		void sizeChanged(const Vector2& size);
	};
}