#pragma once
#include "test.h"
#include "oxygine-framework.h"
#include <sstream>

struct text_test
{
	text_test(
		string str, 
		TextStyle::HorizontalAlign Align, 
		TextStyle::VerticalAlign VAlign, 
		Vector2 Size,
		bool Multiline = true, int Scale2size = 0, bool Html = true):text(str), hAlign(Align), vAlign(VAlign), multiline(Multiline), size(Size), scale2size(Scale2size), html(Html)
	{}

	string text;
	TextStyle::HorizontalAlign hAlign;
	TextStyle::VerticalAlign vAlign;
	bool multiline;
	Vector2 size;
	int scale2size;
	bool html;
};



class TestText: public Test
{
public:
	vector<text_test> tests;
	int current;

	spColorRectSprite bg;
	spColorRectSprite spriteTextRect;
	spTextActor text;

	spTextActor textDetails;

	TestText()
	{
		current = 0;		

		addButton("next", "Next Test");
		addButton("prev", "Prev Test");		

		textDetails = new TextActor;
		textDetails->attachTo(ui);
		textDetails->setSize(Vector2(300, 0));
		textDetails->setPosition(200.0f, getRoot()->getHeight() - 10);
		
		TextStyle st;
		st.font = resourcesUI.getResFont("main")->getFont();
		st.vAlign = TextStyle::VALIGN_BOTTOM;
		st.color = Color(0xffffffff);
		st.multiline = true;
		textDetails->setStyle(st);

		bg = new ColorRectSprite();
		bg->attachTo(content);
		bg->setColor(Color(64, 64, 64, 255));

		/*
		spDragHandler dh = new DragHandler;
		bg->addEventHandler(dh);
		*/

		spriteTextRect = new ColorRectSprite();
		spriteTextRect->attachTo(bg);
		spriteTextRect->setColor(Color(255, 0, 0, 80));

		text = new TextActor;
		text->attachTo(bg);

		Vector2 size(420, 300);

		tests.push_back(text_test( "It is html style tagged text. <div c='ffff00'>It could be colored.  It <div c='0000ff'>supports</div> <div c='00ff00AA'>nested tags</div> and  <br/> broken lines.</div><br/>It supports CDATA and xml escape sequences:\n<![CDATA[<div c='00ffff'>it is CDATA</div>]]>", 
			TextStyle::HALIGN_CENTER, TextStyle::VALIGN_MIDDLE, size, true));

		for (int h = 0; h < 3; ++h)
		{
			tests.push_back(text_test( "<div c='00ff00'>Scaled Font test<br/></div> The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. ", TextStyle::HALIGN_CENTER, TextStyle::VALIGN_MIDDLE, size, true, 10 * h  + 20));
		}


		tests.push_back(text_test( "<div c='00ff00'>Using '\\n' new line test</div>\nnew line\nnew line\nnew line", TextStyle::HALIGN_CENTER, TextStyle::VALIGN_TOP, size));
		
		for (int h = 0; h < 3; ++h)
		{
			tests.push_back(text_test( "<div c='00ff00'>Singleline Test</div> The quick brown fox.", TextStyle::HorizontalAlign(h), TextStyle::VALIGN_TOP, size, false));
		}

		for (int h = 0; h < 3; ++h)
		{
			tests.push_back(text_test( "<div c='00ff00'>Multiline Test with spaces inside</div> The   quick    brown    fox   jumps    over   the    lazy dog.", TextStyle::HorizontalAlign(h), TextStyle::VALIGN_TOP, size));
		}

		

		for (int v = 0; v < 4; ++v)
		{
			for (int h = 0; h < 3; ++h)
			{
				tests.push_back(text_test( "<div c='00ff00'>Aligment Test</div> The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. ", TextStyle::HorizontalAlign(h), TextStyle::VerticalAlign(v), size));
			}
		}

		size = Vector2(200, 100);
		for (int v = 0; v < 4; ++v)
		{
			for (int h = 0; h < 3; ++h)
			{
				tests.push_back(text_test( "<div c='00ff00'>Aligment Test with limited size</div> The quick brown fox jumps over the lazy dog. 1234567890.", TextStyle::HorizontalAlign(h), TextStyle::VerticalAlign(v), size));
			}
		}

		size = Vector2(380, 300);
		for (int h = 0; h < 3; ++h)
		{
			tests.push_back(text_test( "<div c='00ff00'>Long string and aligment test</div> The_quick_brownfox_jumps_over_the lazy dog. 1234567890.", TextStyle::HorizontalAlign(h), TextStyle::VALIGN_TOP, size));
		}	

		updateTest(current);
	}

	void updateTest(int i)
	{
		const text_test &test = tests[i];
		TextStyle style;
		style.multiline = test.multiline;
		style.vAlign = test.vAlign;
		style.hAlign = test.hAlign;
		style.font = resourcesUI.getResFont("big")->getFont();
		style.color = Color(0xffffffff);
		style.fontSize2Scale = test.scale2size;
		text->setStyle(style);

		text->setSize(test.size);
		if (test.html)
			text->setHtmlText(test.text);
		else
			text->setText(test.text);

		//text->setText("<a>a</a>");

		const Rect &rect = text->getTextRect();
		spriteTextRect->setPosition(rect.pos);
		spriteTextRect->setSize(rect.size);

		bg->setSize(test.size);
		bg->setPosition(getSize()/2 - bg->getSize()/2);


		style.font = 0;
		stringstream stream;
		stream << i << ") ";
		stream << dumpStyle(style, false);
		stream << " size=(" << test.size.x << "," << test.size.y << ")";
		textDetails->setText(stream.str());
	}

	void clicked(string id)
	{
		if(id == "next")
		{
			current += 1;
		}
		if(id == "prev")
		{
			current -= 1;
		}

		current += tests.size();
		current %= tests.size();
		updateTest(current);
	}
	
};
