#pragma once
#include "test.h"
#include <map>

DECLARE_SMART(Camera, spCamera);
class Camera : public Actor
{
public:

    spActor _content;

    Camera()
    {
        addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &Camera::onEvent));
        addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Camera::onEvent));
        addEventListener(TouchEvent::MOVE, CLOSURE(this, &Camera::onEvent));
        addEventListener(TouchEvent::WHEEL_DIR, CLOSURE(this, &Camera::onEvent));

        _transform.identity();
    }

    void setContent(spActor content)
    {
        _content = content;
        addChild(content);
    }

    struct touch
    {
        Vector2 current;
        Vector2 previous;
    };

    std::map<int, touch> _touches;

    void onEvent(Event* ev)
    {
        TouchEvent* te = safeCast<TouchEvent*>(ev);
        const Vector2& pos = te->localPosition;



        if (te->type == TouchEvent::TOUCH_DOWN)
        {
            touch& t = _touches[te->index];
            t.previous = t.current = pos;
        }

        if (te->type == TouchEvent::TOUCH_UP)
        {
            auto it = _touches.find(te->index);
            if (it != _touches.end())
                _touches.erase(it);
        }

        if (te->type == TouchEvent::WHEEL_DIR)
        {
            if (te->wheelDirection.y != 0.0f)
            {
                float scale = te->wheelDirection.y < 0 ? 0.95f : 1.05f;

                _transform.translate(-Vector3(pos.x, pos.y, 0));
                _transform.scale(Vector3(scale, scale, 1));
                _transform.translate(Vector3(pos.x, pos.y, 0));
            }
        }

        if (te->type == TouchEvent::MOVE && !_touches.empty())
        {
            touch& t = _touches[te->index];
            t.previous = t.current;

            t.current = pos;

            if (_touches.size() == 1)
            {
                Vector2 offset = t.current - t.previous;
                _transform.translate(Vector3(offset.x, offset.y, 0));
            }
            else
            {
                touch* p1, *p2;
                for (std::map<int, touch>::iterator i = _touches.begin(); i != _touches.end(); ++i)
                {
                    if (i->first == te->index)
                        p1 = &i->second;
                    else
                        p2 = &i->second;
                }

                Vector2 center = (p1->current + p2->current) / 2;
                Vector2 prevCenter = (p1->previous + p2->previous) / 2;

                float dist = p1->current.distance(p2->current);
                float prevDist = p1->previous.distance(p2->previous);
                float scale = dist / prevDist;


                Vector2 offset = center - prevCenter;

                p1->previous = p1->current;
                p2->previous = p2->current;

                _transform.translate(Vector3(offset.x, offset.y, 0));


                _transform.translate(-Vector3(center.x, center.y, 0));
                _transform.scale(Vector3(scale, scale, 1));
                _transform.translate(Vector3(center.x, center.y, 0));
            }
        }

        update();
    }

    void doUpdate(const UpdateState& us)
    {

    }

    void update()
    {
        Transform tr(_transform);
        _content->setTransform(tr);
    }

    Matrix _transform;
};


class TestCamera : public Test
{
public:
    TestCamera()
    {
        spCamera cam = new Camera;
        cam->attachTo(content);
        cam->setSize(content->getSize());


        spSprite map = new Sprite();
        map->setResAnim(resources.getResAnim("map"));
        cam->setContent(map);

        spButton button = new Button;

        button->setPosition(map->getSize() / 2);
        button->setResAnim(resources.getResAnim("button"));
        button->attachTo(map);
        button->setAnchor(0.5f, 0.5f);
        button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestCamera::testClick));

    }

    void testClick(Event* event)
    {
        notify("clicked");
    }
};
