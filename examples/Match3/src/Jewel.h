#pragma once
#include "oxygine-framework.h"

using namespace oxygine;

DECLARE_SMART(Jewel, spJewel);

extern const Vector2 JewelSize;


class Jewel: public Object
{
public:
    Jewel();

    spActor getView() const;
    int     getType() const;

    bool    isExploding() const {return _exploding;}
    bool    isSwaping() const {return _swaping;}
    bool    isDead() const {return _dead;}
    bool    isFalling() const {return _falling;}


    void explode();
    void select();
    void unselect();

    void    fall(const Vector2& pos);
    spTween swap(const Vector2& pos);

private:
    void exploded(Event*);
    void fallen(Event*);
    void swapped(Event*);

    bool _exploding;
    bool _swaping;
    bool _dead;
    bool _falling;
    spSprite _view;

    int _type;
};