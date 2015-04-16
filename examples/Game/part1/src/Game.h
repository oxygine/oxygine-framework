#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

DECLARE_SMART(Player, spPlayer);
DECLARE_SMART(Joystick, spJoystick);
DECLARE_SMART(Game, spGame);
class Game: public Actor
{
public:
    Game();

    void init();

private:
    friend class Player;
    void doUpdate(const UpdateState& us);

    spJoystick _move;

    spPlayer _player;
};