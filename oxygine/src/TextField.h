#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "core/Renderer.h"
#include "TextStyle.h"
#include "VisualStyle.h"
namespace oxygine
{
	DECLARE_SMART(TextField, spTextField);

	namespace text
	{
		class Node;
	}
	

	class TextField : public VStyleActor
	{
	public:
		DECLARE_COPYCLONE_NEW(TextField);
		TextField();
		~TextField();
		
		void serialize(serializedata* data);
		void deserialize(const deserializedata* data);

		/**Returns current text style*/
		const TextStyle&			getStyle() const {return _style;}
		/**Returns text bounds*/
		const Rect&					getTextRect();
		/**Returns current text*/
		const string&				getText() const {return _text;}
		int							getFontSize2Scale() const;
		TextStyle::VerticalAlign	getVAlign() const;
		TextStyle::HorizontalAlign	getHAlign() const;
		bool						getMultiline() const;
		bool						getBreakLongWords () const;

		/**Overwrites TextStyle Vertical align*/
		void setVAlign(TextStyle::VerticalAlign align);
		/**Overwrites TextStyle Horizontal align*/
		void setHAlign(TextStyle::HorizontalAlign align);
		/**Overwrites TextStyle multiline*/
		void setMultiline(bool multiline);
		/**Overwrites TextStyle breakLongWords*/
		void setBreakLongWords(bool val);
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
		void doRender(RenderState const& parentRenderState);

	protected:
		enum
		{
			flag_html = flag_last << 1,
			flag_rebuild = flag_last << 2,
		};
		string  _text;
		TextStyle _style;

		text::Node *_root;
		Rect _textRect;

		text::Node *getStageNode();
		void needRebuild();
		void sizeChanged(const Vector2& size);
	};
}
