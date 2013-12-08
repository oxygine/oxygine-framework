#include "Player.h"
#include "Game.h"
#include "res.h"
#include "Joystick.h"

void Player::_init()
{
	_view->setPosition(_game->getSize()/2);

	_ship = new Sprite;
	_ship->setResAnim(res::ui.getResAnim("ship"));
	_ship->attachTo(_view);
	_ship->setAnchor(Vector2(0.5f, 0.5f));

	_engine = new Sprite;
	_engine->setResAnim(res::ui.getResAnim("shipengine"));
	_engine->attachTo(_ship);
	_engine->addTween(Sprite::TweenColor(Color::Red), 500, -1, true);
	_engine->setVisible(false);
}

void Player::_update(const UpdateState &us)
{
	_engine->setVisible(false);
	Vector2 dir;
	if (_game->_move->getDirection(dir))
	{
		Vector2 pos = _view->getPosition();
		float angle = atan2f(dir.y, dir.x);
		_view->setRotation(angle);
		pos = pos + dir * (us.dt / 1000.0f) * 5;
		_view->setPosition(pos);

		_engine->setVisible(true);
	}
}