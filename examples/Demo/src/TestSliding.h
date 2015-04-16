#pragma once
#include "test.h"
#include "ClipRectActor.h"
#include "PointerState.h"
#include "SlidingActor.h"

class TestSliding: public Test
{
public:
    spSlidingActor _sliding;
    bool _snapEnabled;
    TestSliding()
    {
        _snapEnabled = false;

        spSlidingActor sliding = new SlidingActor();
        sliding->addEventListener(SlidingActor::SlidingEvent::BEGIN, CLOSURE(this,  &TestSliding::slideBegin));
        sliding->addEventListener(SlidingActor::SlidingEvent::SLIDING, CLOSURE(this,  &TestSliding::sliding));
        sliding->addEventListener(SlidingActor::SlidingEvent::END, CLOSURE(this,  &TestSliding::slideEnd));


        sliding->setSize(getWidth() / 3 * 2, getHeight() / 3 * 2);


        spColorRectSprite content = new ColorRectSprite();
        content->setSize(sliding->getWidth(), 2000);
        content->setColor(Color(200, 200, 200, 255));
        for (int i = 0; i < 20; i += 2)
        {
            spColorRectSprite c = initActor(new ColorRectSprite,
                                            arg_size = Vector2(content->getWidth(), 100),
                                            arg_color = Color(164, 164, 164, 255),
                                            arg_y = i * 100.0f,
                                            arg_attachTo = content);

            c->setCull(true);//cull invisible parts

            spButton button = initActor(new Button,
                                        arg_resAnim = resourcesUI.getResAnim("button"),
                                        arg_attachTo = c);
            button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestSliding::testClick));
        }

        spTextField title = initActor(new TextField,
                                      arg_text = "Sliding demo",
                                      arg_w = content->getWidth(),
                                      arg_vAlign = TextStyle::VALIGN_TOP,
                                      arg_hAlign = TextStyle::HALIGN_CENTER,
                                      arg_font = resourcesUI.getResFont("big")->getFont(),
                                      arg_color = Color(0xFF0000ff),
                                      arg_attachTo = content);

        sliding->setContent(content);
        sliding->setPosition(getSize() / 2 - sliding->getSize() / 2);
        sliding->attachTo(this->content);

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
        }
    }

    void slideBegin(Event* event)
    {
        notify("slideBegin");
        SlidingActor::SlidingEvent* sd = safeCast<SlidingActor::SlidingEvent*>(event);
    }

    void slideEnd(Event* event)
    {
        notify("slideEnd");

        if (!_snapEnabled)
            return;

        //snap to  grid
        SlidingActor::SlidingEvent* sd = safeCast<SlidingActor::SlidingEvent*>(event);
        spActor content = _sliding->getContent();
        float y = content->getY();
        y = int(y / 100) * 100.0f;
        Vector2 dest(content->getX(), y);
        content->addTween(Actor::TweenPosition(dest), 100);
    }

    void sliding(Event* event)
    {
        /*
        //snap
        SlidingActor::SlidingEvent* sd = safeCast<SlidingActor::SlidingEvent *>(event);
        if (sd->speed.sqlength() < 2500)
        {
            sd->speed = Vector2(0,0);
        }
        */
    }

    void testClick(Event* event)
    {
        notify("clicked");
    }
};
