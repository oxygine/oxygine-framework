#pragma once
#include "test.h"
#include "ClipRectActor.h"
class TestClipRect: public Test
{
public:
    TestClipRect()
    {
        spClipRectActor clp = createClipRect(250, 250, 4.0f);
        clp->attachTo(content);
        clp->setPosition(content->getSize() / 2 - clp->getSize() / 2);

        spClipRectActor childClp = createClipRect(100, 100, 2.0f);
        childClp->setPosition(-40.0f, -40.0f);
        childClp->attachTo(clp);

        spSprite spr = new Sprite;
        spr->setResAnim(resources.getResAnim("batterfly"));
        spr->setX(200.0f);
        spr->attachTo(clp);
    }

    spClipRectActor createClipRect(int w, int h, float sc)
    {
        spClipRectActor clp = new ClipRectActor;
        clp->setSize((float)w, (float)h);


        spSprite spr = new Sprite;
        spr->setPosition(w / 2.0f, h / 2.0f);
        spr->setResAnim(resources.getResAnim("batterfly"));
        spr->attachTo(clp);
        spr->setAnchor(0.5f, 0.5f);
        spr->setScale(sc);
        spr->addTween(Actor::TweenRotation(MATH_PI * 2), 5000, -1);

        return clp;
    }

    void onMouseDown(Event* event)
    {
        spActor actor = safeSpCast<Actor>(event->currentTarget);

        int new_priority = actor->getParent()->getLastChild()->getPriority() + 1;
        actor->setPriority(new_priority);

        actor->addTween(Sprite::TweenColor(Color(Color::Red, 255)), 300, -1, true);
    }

    void onMouseUp(Event* event)
    {
        spSprite actor = safeSpCast<Sprite>(event->currentTarget);
        actor->removeTweens();
        actor->setColor(Color::White);
    }
};
