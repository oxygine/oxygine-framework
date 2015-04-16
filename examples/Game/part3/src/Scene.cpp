#include "Scene.h"

Scene::Scene()
{
    _view = new Actor;
    _view->setSize(getStage()->getSize());
}

void Scene::changeScene(spScene next)
{
    //hide current scene
    spTween tween = _view->addTween(Actor::TweenAlpha(0), 300);
    //detach when done
    tween->setDetachActor(true);

    //show next scene
    getStage()->addChild(next->getView());
    next->getView()->setAlpha(0);
    next->getView()->addTween(Actor::TweenAlpha(255), 300);
}