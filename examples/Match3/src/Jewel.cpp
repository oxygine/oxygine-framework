#include "Jewel.h"
#include "shared.h"

const Vector2 JewelSize(100, 100);

Jewel::Jewel(): _swaping(false), _exploding(false), _dead(false), _falling(false)
{
    _type = rand() % 6 + 1;

    _view = new Sprite;

    char name[255];
    safe_sprintf(name, "jewel%d", _type);
    ResAnim* rs = res.getResAnim(name);

    _view->setResAnim(rs);


    //set pilot to center of sprite
    Vector2 center = _view->getSize() / 2;
    //round center to avoid blurred image
    center.x = int(center.x);
    center.y = int(center.y);
    _view->setAnchorInPixels(center);
}

spActor Jewel::getView() const
{
    return _view;
}

int     Jewel::getType() const
{
    return _type;
}

void Jewel::explode()
{
    _exploding = true;
    float angle = 360 * (rand() % 2 ? -1 : 1);
    spTween tween = _view->addTween(Actor::TweenRotationDegrees(angle), 500);
    _view->addTween(Actor::TweenAlpha(0), 500);
    tween->setDoneCallback(CLOSURE(this, &Jewel::exploded));
}

void Jewel::exploded(Event*)
{
    _dead = true;
    _view->detach();
}

void Jewel::select()
{
    _view->addTween(Actor::TweenRotation(1), 500, -1, true);
}

void Jewel::unselect()
{
    _view->removeTweens(false);
    _view->addTween(Actor::TweenRotation(0), 250);
}

void Jewel::fall(const Vector2& pos)
{
    _falling = true;
    _view->addTween(Actor::TweenPosition(pos), 300) ->setDoneCallback(CLOSURE(this, &Jewel::fallen));
}

spTween Jewel::swap(const Vector2& pos)
{
    _swaping = true;
    spTween tween = _view->addTween(Actor::TweenPosition(pos), 200);
    tween->addDoneCallback(CLOSURE(this, &Jewel::swapped));
    return tween;
}

void Jewel::swapped(Event*)
{
    _swaping = false;
}

void Jewel::fallen(Event*)
{
    _falling = false;
}