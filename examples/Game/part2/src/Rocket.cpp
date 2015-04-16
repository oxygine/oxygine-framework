#include "Rocket.h"
#include "res.h"
#include "Game.h"
#include "Enemy.h"

Rocket::Rocket(const Vector2& dir): _dir(dir)
{
}

void Rocket::_init()
{
    //initialize rocket's sprite
    spSprite sp = new Sprite;
    sp->setResAnim(res::ui.getResAnim("rocket"));
    sp->setAnchor(Vector2(0.5f, 0.5f));
    sp->setScale(0);
    sp->addTween(Sprite::TweenScale(1.0f), 500);

    _view->addChild(sp);
    _view->setRotation(atan2f(_dir.y, _dir.x));
}

void Rocket::_update(const UpdateState& us)
{
    //move rocket by it's direction each frame
    Vector2 pos = _view->getPosition();
    pos += _dir * (us.dt / 1000.0f) * 500.0f;
    _view->setPosition(pos);

    //find intersection with Enemies and explode them
    for (Game::units::iterator i = _game->_units.begin(); i != _game->_units.end(); ++i)
    {
        spUnit unit = *i;
        //list of units has everything, but we need only Enemies
        spEnemy enemy = dynamic_cast<Enemy*>(unit.get());
        if (!enemy)
            continue;

        Vector2 d = unit->getPosition() - pos;
        if (d.length() < 20)
        {
            //if rocket is too close to Enemy then try to explode it and explode rocket
            enemy->explode();
            explode();

            return;
        }
    }


    //if rocked out of bounds then explode it
    RectF bounds(0, 0, _game->getWidth(), _game->getHeight());
    if (!bounds.pointIn(pos))
    {
        explode();
    }
}

void Rocket::explode()
{
    //we are dead
    //set this flag to true and it this rocket would be removed from units list in Game::doUpdate
    _dead = true;

    //create explode sprite
    spSprite anim = new Sprite;
    anim->attachTo(_game);
    anim->setBlendMode(blend_add);
    anim->setPosition(_view->getPosition());
    anim->setAnchor(Vector2(0.5f, 0.5f));

    //run tween with explosion animation
    spTween tween = anim->addTween(Sprite::TweenAnim(res::ui.getResAnim("explosion")), 1000);
    //auto detach sprite when tween is done
    tween->setDetachActor(true);

    //hide rocket and then detach it
    tween = _view->addTween(Actor::TweenAlpha(0), 500);
    tween->setDetachActor(true);
}