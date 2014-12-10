#include "Joystick.h"
#include "res.h"

Joystick::Joystick():_pressed(false), _dir(0,0)
{
	setResAnim(res::ui.getResAnim("joystick"));
	setAlpha(128);

	//handle touch events
	addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &Joystick::onEvent));
	addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Joystick::onEvent));
	addEventListener(TouchEvent::MOVE, CLOSURE(this, &Joystick::onEvent));

	_finger = new Sprite;
	_finger->setResAnim(res::ui.getResAnim("finger"));
	_finger->attachTo(this);
	_finger->setVisible(false);
	_finger->setAnchor(Vector2(0.5f, 0.5f));
	_finger->setTouchEnabled(false);
}

void Joystick::onEvent(Event *ev)
{
	TouchEvent *te = safeCast<TouchEvent *>(ev);

	//if player touched down
	if (te->type == TouchEvent::TOUCH_DOWN)
	{
		_finger->setVisible(true);
		setColor(Color::Red);
		_pressed = true;
	}

	//if player touched up
	if (te->type == TouchEvent::TOUCH_UP)
	{
		_finger->setVisible(false);
		setColor(Color::White);
		_pressed = false;
	}

	if (te->type == TouchEvent::MOVE)
	{
	}

	Vector2 center = getSize()/2;
	_dir = te->localPosition - center;

	if (_dir.length() > 100)
		_dir.normalizeTo(100);

	_finger->setPosition(center + _dir);

	if (!_pressed)
	{
		_dir = Vector2(0,0);
	}
}

bool Joystick::getDirection(Vector2 &dir) const
{
	dir = _dir;
	return _pressed;
}