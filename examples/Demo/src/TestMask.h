#pragma once
#include "test.h"
#include "TestPolygon.h"


class TestMask: public Test
{
public:
    timeMS _lastSnow;

    spMaskedSprite _masked;
    spSprite _mask;

    TestMask(): _lastSnow(0)
    {
        spSprite sp = new Sprite;
        sp->attachTo(content);
        sp->setResAnim(resources.getResAnim("sky"));

        _mask = new Sprite;
        _mask->setScale(1.5f);
        _mask->setPriority(1);
        _mask->setAlpha(100);
        _mask->setX(100);
        _mask->setY(50);
        _mask->setAnchor(0.5f, 0.5f);
        _mask->setResAnim(resources.getResAnim("mask"));
        _mask->setVisible(true);
        _mask->attachTo(content);

        _mask->addTween(Actor::TweenRotation(MATH_PI * 2), 15000, -1, true);
        _mask->addTween(Actor::TweenX(content->getWidth() - 100), 10000, -1, true);
        _mask->addTween(Actor::TweenY(content->getHeight() - 50), 12000, -1, true);

        _masked = new MaskedSprite;
        _masked->attachTo(content);

        _masked->setMask(_mask, true);


        spPolygon poly = new oxygine::Polygon;

        vertexPCT2* data = TestPolygon::createVertices(10);
        poly->setVertices(data, sizeof(vertexPCT2) * 10 * 4, vertexPCT2::FORMAT, true);
        poly->setPosition(getSize());
        _masked->addChild(poly);


        TextStyle style;
        style.font = _resources.getResFont("big");
        style.color = Color::OrangeRed;
        style.vAlign = TextStyle::VALIGN_MIDDLE;
        style.hAlign = TextStyle::HALIGN_MIDDLE;
        style.multiline = true;

        spTextField text = new TextField();
        text->attachTo(_masked);
        text->setPosition(content->getSize() / 2);
        text->setText("Oxygine. Masked sky demo");
        text->setStyle(style);

        text->addTween(TweenOutline(Color::Green), 3000, -1, true);

        const Test::toggle sm[] = {Test::toggle("hide mask", 0), Test::toggle("show mask", 1), };
        addToggle("show_mask", sm, 2);

        const Test::toggle cm[] = {Test::toggle("change mask1", 0), Test::toggle("change mask2", 1), };
        addToggle("change_mask", cm, 2);

        const Test::toggle dm[] = {Test::toggle("disable mask", 0), Test::toggle("enable mask", 1), };
        addToggle("disable_mask", dm, 2);

        const Test::toggle pause[] = {Test::toggle("pause", 0), Test::toggle("resume", 1), };
        addToggle("pause", pause, 2);
    }

    void toggleClicked(string id, const toggle* t)
    {
        if (id == "show_mask")
            _mask->setVisible(t->value != 0);

        if (id == "disable_mask")
        {
            if (t->value == 0)
                _masked->setMask(0);
            else
                _masked->setMask(_mask, true);
        }

        if (id == "pause")
        {
            spClock clock = getStage()->getClock();
            if (t->value == 0)
                clock->pause();
            else
                clock->resume();
        }

        if (id == "change_mask")
        {
            _mask->removeTweensByName("tweenanim");
            switch (t->value)
            {
                case 1:
                    _mask->setResAnim(resources.getResAnim("mask"));
                    _masked->setMask(_mask, true);
                    break;
                case 0:
                    _mask->addTween(TweenAnim(resources.getResAnim("anim")), 600, -1, false)->setName("tweenanim");
                    _masked->setMask(_mask, false);
                    break;
            }
        }
    }


    void doUpdate(const UpdateState& us)
    {
        if (_lastSnow  + 20 < us.time)
        {
            _lastSnow = us.time;
            spSprite snow = new Sprite;
            snow->setAnchor(0.5f, 0.5f);
            snow->setResAnim(resources.getResAnim("snow"));
            snow->setRotation(scalar::randFloat(0, MATH_PI * 2));
            snow->setScale(scalar::randFloat(0.4f, 1.0f));
            snow->setX(scalar::randFloat(0, content->getWidth()));
            snow->setY(-50);
            snow->attachTo(_masked);

            snow->addTween(Actor::TweenY(content->getHeight() + 50), 6000)->detachWhenDone();
            snow->addTween(Actor::TweenRotation(scalar::randFloat(0, MATH_PI * 2)), 5000);
        }
    }
};
