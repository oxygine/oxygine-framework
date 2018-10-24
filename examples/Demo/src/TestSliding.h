#pragma once
#include "test.h"

class TestSliding: public Test
{
public:
    spSlidingActor  _sliding;
    spSprite        _map;
    spButton        _button;

    bool _snapEnabled;
    TestSliding()
    {
        _snapEnabled = false;

        spSlidingActor sliding = new SlidingActor();
        sliding->addEventListener(SlidingActor::SlidingEvent::BEGIN, CLOSURE(this,  &TestSliding::slideBegin));
        sliding->addEventListener(SlidingActor::SlidingEvent::SLIDING, CLOSURE(this,  &TestSliding::sliding));
        sliding->addEventListener(SlidingActor::SlidingEvent::END, CLOSURE(this,  &TestSliding::slideEnd));


        sliding->setSize(getWidth() / 3 * 2, getHeight() / 3 * 2);


        spSprite map = new Sprite();
        map->setResAnim(resources.getResAnim("map"));
        map->setPosition(sliding->getSize() / 2 - map->getSize() / 2);
        _map = map;

        spButton button = new Button;
        _button = button;
        button->setPosition(map->getSize() / 2);
        button->setResAnim(DebugActor::resSystem->getResAnim("button"));
        button->attachTo(map);
        button->setAnchor(0.5f, 0.5f);
        button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestSliding::testClick));


        spTextField title = new TextField;
        title->setText("Sliding Demo");
        title->setSize(map->getSize());
        title->setAlign(TextStyle::VALIGN_MIDDLE, TextStyle::HALIGN_MIDDLE);
        title->setY(-50);
        title->setFont(_resources.getResFont("big"));
        title->setColor(Color::Black);
        title->attachTo(map);

        sliding->setContent(map);
        sliding->setPosition(getSize() / 2 - sliding->getSize() / 2);
        sliding->attachTo(_content);

        _sliding = sliding;

        toggle tl[] = {toggle("lock", 0), toggle("unlock", 1)};
        addToggle("lock", tl, 2);

        toggle ts[] = {toggle("enable snap", 0), toggle("disable snap", 1)};
        addToggle("snap", ts, 2);

        addButton("stop", "stop");
    }

    void clicked(string id)
    {
        if (id == "stop")
        {
            _sliding->stop();
        }
    }

    void toggleClicked(string id, const toggle* data)
    {
        if (id == "lock")
            _sliding->setLocked(data->value == 0);
        if (id == "snap")
        {
            _snapEnabled = data->value == 0;

            snap();
        }
    }

    void slideBegin(Event* event)
    {
        notify("slideBegin");
        //SlidingActor::SlidingEvent* sd = safeCast<SlidingActor::SlidingEvent*>(event);
    }

    void snap()
    {
        if (!_snapEnabled)
            return;

        //snap to  grid
        spActor content = _sliding->getContent();
        float y = content->getY();
        y = int(y / 169) * 169.0f;

        float x = content->getX();
        x = int(x / 166) * 166.0f;
        Vector2 dest(x, y);
        content->addTween(Actor::TweenPosition(dest), 100);
    }

    void slideEnd(Event* event)
    {
        notify("slideEnd");

        snap();
    }

    void sliding(Event* event)
    {

    }

    void testClick(Event* event)
    {
        notify("clicked");
        _button->addTween(Sprite::TweenColor(Color::Red), 300, 1, true);
    }
};
