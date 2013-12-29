#include "MyButton.h"

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