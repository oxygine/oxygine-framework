#pragma once
#include "test.h"

class TestTouches: public Test
{
public:
    spTextField txtMove;
    spTextField txtClick;
    spTextField txtUp;
    float y;
    int n;

    spTextField click;

    spSprite createRect(const char* name, const Color& color, const Vector2& pos, const Vector2& size)
    {
        spSprite orange = new ColorRectSprite;
        orange->setColor(color);
        orange->setName(name);
        orange->setSize(size);
        orange->setPosition(pos);
        orange->attachTo(content);
        //orange->setAlpha(200);
        orange->addEventListener(TouchEvent::OVER, CLOSURE(this, &TestTouches::onOver));
        orange->addEventListener(TouchEvent::OUT, CLOSURE(this, &TestTouches::onOver));

        spTextField tf = new TextField;
        tf->setText("out");
        tf->setColor(Color::Black);
        tf->setName("txt");
        tf->setX(10);
        tf->setY(5);
        tf->attachTo(orange);

        return orange;
    }

    void onOver(Event* ev)
    {
        spSprite s = safeSpCast<Sprite>(ev->currentTarget);
        spTextField tf = s->getChildT<TextField>("txt");
        tf->setText(ev->type == TouchEvent::OVER ? "over" : "out");
    }


    TestTouches()
    {
        y = 0;
        n = 0;

        txtClick = createTF(Color::White);
        txtUp = createTF(Color::Green);
        txtMove = createTF(Color::Yellow);


        spSprite Orange = createRect("Orange", Color::Orange, Vector2(200, 200), Vector2(300, 300));
        Orange->attachTo(content);

        spSprite Green = createRect("Green", Color::Green, Vector2(100, 25), Vector2(100, 100));
        Green->attachTo(Orange);

        spSprite Beige = createRect("Beige", Color::Beige, Vector2(150, 150), Vector2(250, 100));
        Beige->attachTo(Orange);

        spSprite LightGreen = createRect("LightGreen", Color::LightGreen, Vector2(180, -50), Vector2(50, 200));
        LightGreen->attachTo(Beige);


        spActor target = Orange;

        target->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestTouches::event));
        target->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestTouches::event));
        target->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &TestTouches::event));
    }

    spTextField createTF(const Color& c)
    {
        spTextField tf = new TextField;
        tf->attachTo(content);
        tf->setWidth(300);
        tf->setColor(c);
        tf->setX(content->getWidth() - tf->getWidth());
        tf->setY(y);
        tf->setMultiline(true);
        tf->setFont(Test::resourcesUI.getResFont("big")->getFont());
        y += 200;
        return tf;
    }


    void event(Event* ev)
    {
        TouchEvent* te = safeCast<TouchEvent*>(ev);


        spTextField tf;
        const char* eventName = "";
        if (te->type == TouchEvent::CLICK)
        {
            tf = txtClick;
            eventName = "CLICK";
            notify("click");
        }

        if (te->type == TouchEvent::MOVE)
        {
            tf = txtMove;
            eventName = "MOVE";
        }

        if (te->type == TouchEvent::TOUCH_UP)
        {
            tf = txtUp;
            eventName = "UP";
            notify("up");
        }

        char str[512];
        safe_sprintf(str,
                     "%d)touch event: %s\n"
                     "target: %s\n"
                     "position: %d %d\n"
                     "\n"
                     "current target: %s\n"
                     "local position: %d %d\n"
                     ,
                     n,
                     eventName,
                     te->target->getName().c_str(),
                     (int)te->position.x, (int)te->position.y,
                     te->currentTarget->getName().c_str(),
                     (int)te->localPosition.x, (int)te->localPosition.y

                    );


        tf->setText(str);
        ++n;
    }
};
