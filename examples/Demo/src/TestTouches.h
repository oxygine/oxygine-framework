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
        orange->addEventListener(TouchEvent::OUTX, CLOSURE(this, &TestTouches::onOver));
        orange->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &TestTouches::onDownUp));
        orange->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &TestTouches::onDownUp));
        orange->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestTouches::onMove));

        spTextField tf = new TextField;
        tf->setText("");
        tf->setColor(Color::Black);
        tf->setName("state1");
        tf->setX(5);
        tf->setY(5);
        tf->attachTo(orange);

        for (int i = 0; i < MouseButton_Num; ++i)
        {
            tf = new TextField;
            tf->setText("");
            tf->setColor(Color::Black);
            char name[255];
            safe_sprintf(name, "pressed %d", i);
            tf->setName(name);
            tf->setX(5.0f);
            tf->setY(15.0f + i * 10.0f);
            tf->attachTo(orange);
        }




        tf = new TextField;
        tf->setText("");
        tf->setColor(Color::Black);
        tf->setName("local");
        tf->setX(5);
        tf->setY(50);
        tf->setMultiline(true);
        tf->setWidth(orange->getWidth());
        tf->attachTo(orange);

        return orange;
    }

    void onOver(Event* ev)
    {
        spSprite s = safeSpCast<Sprite>(ev->currentTarget);
        spTextField tf = s->getChildT<TextField>("state1");
        tf->setText(ev->type == TouchEvent::OVER ? "over" : "");
        updateLocalPos(ev);
    }

    void onDownUp(Event* ev)
    {
        TouchEvent* te = safeCast<TouchEvent*>(ev);
        spSprite s = safeSpCast<Sprite>(ev->currentTarget);

        char name[255];
        safe_sprintf(name, "pressed %d", (int)te->mouseButton);

        spTextField tf = s->getChildT<TextField>(name);
        tf->setText(ev->type == TouchEvent::TOUCH_DOWN ? name : "");
        updateLocalPos(ev);
    }

    void updateLocalPos(Event* ev)
    {
        spSprite s = safeSpCast<Sprite>(ev->currentTarget);
        spTextField tf = s->getChildT<TextField>("local");

        if (ev->type != TouchEvent::OUTX)
        {
            TouchEvent* te = safeCast<TouchEvent*>(ev);
            char str[255];
            safe_sprintf(str, "loc.pos: %d,%d", (int)te->localPosition.x, (int)te->localPosition.y);
            tf->setText(str);
        }
        else
            tf->setText("");
    }
    void onMove(Event* ev)
    {
        updateLocalPos(ev);
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

        spSprite Green = createRect("Green", Color::Green, Vector2(100, 25), Vector2(100, 150));
        Green->attachTo(Orange);

        spSprite Beige = createRect("Beige", Color::Beige, Vector2(150, 150), Vector2(260, 100));
        Beige->attachTo(Orange);

        spSprite LightGreen = createRect("LightGreen", Color::LightGreen, Vector2(180, -50), Vector2(65, 200));
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
        tf->setFont(_resources.getResFont("big"));
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
                     "button: %d\n"
                     ,
                     n,
                     eventName,
                     te->target->getName().c_str(),
                     (int)te->position.x, (int)te->position.y,
                     te->currentTarget->getName().c_str(),
                     (int)te->localPosition.x, (int)te->localPosition.y,
                     (int)te->mouseButton
                    );


        tf->setText(str);
        ++n;
    }
};
