#include "Scene.h"

Scene::Scene()
{
    _view = new Actor;
    _view->setSize(getStage()->getSize());
}

void Scene::changeScene(spScene next)
{
    //hide current scene
    hide();

    //show next scene
    next->show();
}

void Scene::show()
{
    //add scene view to root
    getStage()->addChild(_view);
    //and fade in
    _view->setAlpha(0);
    _view->addTween(Actor::TweenAlpha(255), 300);

    //call virtual method (overloaded in inherited classes)
    _show();
}

void Scene::hide()
{
    spTween tween = _view->addTween(Actor::TweenAlpha(0), 300);
    //detach when done
    tween->setDetachActor(true);
    //and call Scene::hidden
    tween->addDoneCallback(CLOSURE(this, &Scene::hidden));
}

void Scene::hidden(Event* ev)
{
    //hidden called by Tween
    //notify HiddenEvent listeners

    HiddenEvent he(this);
    dispatchEvent(&he);
}