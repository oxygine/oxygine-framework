#pragma once
#include "oxygine-framework.h"
#include <list>
using namespace oxygine;

DECLARE_SMART(Player, spPlayer);
DECLARE_SMART(Joystick, spJoystick);
DECLARE_SMART(Game, spGame);
DECLARE_SMART(Rocket, spRocket);
DECLARE_SMART(Unit, spUnit);

class Game: public Actor
{
public:
    Game();
    ~Game();

    void init();

protected:
    friend class Rocket;
    friend class Player;
    friend class Unit;

    void doUpdate(const UpdateState& us);

    spActor _ui;
    spJoystick _move;
    spJoystick _fire;

    spPlayer _player;

    typedef std::list<spUnit> units;
    units _units;
};