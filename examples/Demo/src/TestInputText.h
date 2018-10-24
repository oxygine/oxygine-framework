#pragma once
#include "test.h"


DECLARE_SMART(TextWithBackground, spTextWithBackground);
class TextWithBackground: public ColorRectSprite
{
public:
    TextWithBackground(const string& defText)
    {
        text = new TextField;
        //Don't handle touch events on this Actor
        text->setTouchEnabled(false);

        TextStyle style;
        style.color = Color::Black;
        style.hAlign = TextStyle::HALIGN_MIDDLE;
        style.vAlign = TextStyle::VALIGN_MIDDLE;
        style.multiline = true;
        style.font = Test::_resources.getResFont("big");
        text->setStyle(style);
        text->setText(defText);

        addChild(text);
    }

    spTextField text;

    void sizeChanged(const Vector2& size)
    {
        text->setSize(size);
    }
};

class TestInputText: public Test
{
public:
    spInputText _input;
    spTextWithBackground _current;

    TestInputText()
    {
        _input = new InputText;
        //_input->setAllowedSymbols("1234567890");
        //_input->setDisallowedSymbols("0");
        _input->addEventListener(Event::COMPLETE, CLOSURE(this, &TestInputText::onComplete));

        spTextWithBackground t = new TextWithBackground("click and edit me 1");
        t->setSize(200, 60);
        t->setPosition(_content->getWidth() / 2 - t->getWidth() / 2, 100);
        t->attachTo(this);
        t->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestInputText::onClick));

        t = new TextWithBackground("click and edit me 2");
        t->setSize(200, 60);
        t->setPosition(_content->getWidth() / 2 - t->getWidth() / 2, 170);
        t->attachTo(this);
        t->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestInputText::onClick));
    }

    void onClick(Event* ev)
    {
        if (_current)
        {
            _current->setColor(Color::White);
        }

        _current = safeSpCast<TextWithBackground>(ev->currentTarget);
        _input->start(_current->text);
        _current->setColor(Color::Red);
    }

    void onComplete(Event* ev)
    {
        if (_current)
        {
            _current->setColor(Color::White);
            notify(_current->text->getText());
        }
        _current = 0;
        InputText::stopAnyInput();
    }

    ~TestInputText()
    {
        InputText::stopAnyInput();
    }

    void clicked(string id)
    {

    }
};
