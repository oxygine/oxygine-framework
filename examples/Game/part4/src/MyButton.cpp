#include "MyButton.h"
#include "res.h"

MyButton::MyButton()
{
	//pressed button should be RED
	addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &MyButton::onEvent));
	addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &MyButton::onEvent));
	addEventListener(TouchEvent::CLICK, CLOSURE(this, &MyButton::onEvent));
}

void MyButton::onEvent(Event *ev)
{
	TouchEvent *event = static_cast<TouchEvent *>(ev);
	if (ev->type == TouchEvent::TOUCH_DOWN)
		setColor(Color::Red);

	if (ev->type == TouchEvent::TOUCH_UP)
		setColor(Color::White);

	if (ev->type == TouchEvent::CLICK)
	{
		//clicked button should scale up and down
		setScale(1.0f);
		addTween(Actor::TweenScale(1.1f), 300, 1, true);
	}
}

void MyButton::setText(const string &txt)
{
	if (!_text)
	{		
		//create TextActor if it wasn't created yet
		TextStyle style;
		style.font = res::ui.getResFont("normal")->getFont();
		style.vAlign = TextStyle::VALIGN_MIDDLE;
		style.hAlign = TextStyle::HALIGN_CENTER;

		//attach it to MyButton and set the same size as button
		//text would be centered
		_text = initActor(new TextActor,
			arg_style = style,
			arg_size = getSize(),
			arg_attachTo = this);
	}

	_text->setText(txt);
}