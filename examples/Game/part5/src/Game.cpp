#include "Game.h"
#include "Joystick.h"
#include "Player.h"
#include "res.h"
#include "Enemy.h"

Game::Game()
{

}

Game::~Game()
{

}

void Game::init()
{
    //scene layer would have size of display
    setSize(getStage()->getSize());

    //create background
    spSprite sky = new Sprite;
    sky->setResAnim(res::ui.getResAnim("sky"));
    sky->attachTo(this);
    sky->setSize(getSize());

    //create separate layer for elements virtual joystick and other UI in future
    _ui = new Actor;
    _ui->attachTo(this);
    //it would be higher than other actors with default priority = 0
    _ui->setPriority(1);

    //create virtual joystick and attach it to UI
    _move = new Joystick;
    _move->attachTo(_ui);
    _move->setY(getHeight() - _move->getHeight());

    //create virtual joystick and attach it to UI
    _fire = new Joystick;
    _fire->attachTo(_ui);
    _fire->setX(getWidth() - _fire->getWidth());
    _fire->setY(getHeight() - _fire->getHeight());


    //create enemies
    for (int i = 0; i < 10; ++i)
    {
        spEnemy enemy = new Enemy;
        enemy->init(Vector2(scalar::randFloat(0, getWidth()), scalar::randFloat(0, getHeight())), this);
    }


    //create player ship
    _player = new Player;
    _player->init(getSize() / 2, this);

}

void Game::doUpdate(const UpdateState& us)
{
    //update all units
    //ship, rocket and enemies are in this list
    for (units::iterator i = _units.begin(); i != _units.end();)
    {
        spUnit unit = *i;
        unit->update(us);

        if (unit->isDead())
        {
            //it is dead. Time to remove it from list
            i = _units.erase(i);
        }
        else
        {
            ++i;
        }
    }
}