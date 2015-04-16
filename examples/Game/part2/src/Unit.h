#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

class Game;
DECLARE_SMART(Unit, spUnit);
class Unit: public Object
{
public:
    Unit();

    void init(const Vector2& pos, Game* game);

    const Vector2& getPosition() const;

    void update(const UpdateState& us);
    bool isDead() const {return _dead;}

protected:
    virtual void _init() {}
    virtual void _update(const UpdateState& us) {}

    spActor _view;
    Game* _game;
    bool _dead;
};