#include "ModalActor.h"
#include "Blocking.h"
#include "Actor.h"

ModalActor::ModalActor():_timefadeIn(250), _timeFadeOut(250)
{
	_actor = new Actor;
}

ModalActor::~ModalActor()
{

}

void ModalActor::showing()
{
	_actor->setInputEnabled(false);
	Modal::showing();
	_actor->setInputEnabled(true);
}

void ModalActor::hiding()
{
	_actor->setInputEnabled(false);
	Modal::hiding();
	_actor->setInputEnabled(true);
}

void ModalActor::doShowing()
{
	_actor->setAlpha(0);
	spTween t = createTween(Actor::TweenAlpha(255), _timefadeIn);
	_actor->addTween(t);
	blocking::waitTween(t);
	
}

void ModalActor::doHiding()
{
	spTween t = createTween(Actor::TweenAlpha(0), _timeFadeOut);
	_actor->addTween(t);
	blocking::waitTween(t);
}