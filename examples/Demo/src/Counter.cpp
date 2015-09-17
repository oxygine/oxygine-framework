#include "Counter.h"
#include "utils/stringUtils.h"
#include "Sprite.h"
#include "res/Resources.h"

DECLARE_SMART(Number, spNumber);
class Number : public Actor
{
public:
    enum { ADDED = eventID('N', 'm', 'T', 'r') };
    Number(const Vector2& size, int pos) : _current(0), _pos(pos), _running(false)
    {
        _line = new Actor;
        _line->setUserData(this);
        addChild(_line);
        setSize(size);

    }

    int getPos() const { return _pos; }

    void setActor(int n, spActor a)
    {
        _sprites[n] = a;
        a->setUserData((void*)n);
    }

    int getCurrent() const
    {
        return _current;
    }

    spTween add()
    {
        OX_ASSERT(_running == false);

        _running = true;
        int next = (_current + 1) % 10;
        spActor actor = _sprites[next];
        push(actor);
        float offset = getHeight();
        spTween tween = _line->addTween(Actor::TweenY(-offset), 500, 1, false, 0, Tween::ease_inOutCubic);
        tween->setDoneCallback(CLOSURE(this, &Number::done));

        return tween;
    }

    void done(Event*)
    {
        _running = false;
        _line->removeChild(_line->getFirstChild());
        _line->setY(0);
        _line->getFirstChild()->setY(0);
        _current = (_current + 1) % 10;
    }


    float push(spActor s)
    {
        spActor last = _line->getLastChild();
        if (last)
            s->setY(last->getY() + getHeight());
        else
            s->setY(0.0f);

        s->setX(getWidth() / 2 - s->getWidth() / 2);

        _line->addChild(s);

        return last ? last->getHeight() : getHeight();
    }

    void reset()
    {
        _running = false;
        _line->removeChildren();
        _line->removeTweens();
        _line->setPosition(0, 0);
        _current = 0;

        if (_pos != 0)
        {
            spActor a = new Actor;
            a->setSize(getSize());
            push(a);
        }
        else
        {
            push(_sprites[0]);
        }
    }

private:
    spActor _line;
    spActor _sprites[10];

    int _current;
    int _pos;
    bool _running;
};


Counter::Counter()
{

}

Counter::~Counter()
{

}

void Counter::init(const Resources* res, float numberWidth)
{
    _dest = 0;
    _src = 0;
    _running = false;

    Vector2 sz(numberWidth, getHeight());
    for (int n = 0; n < NUM; ++n)
    {
        spNumber num = new Number(sz, n);
        _numbers[n] = num;
        addChild(num);
        num->setPosition(getWidth() - sz.x - n * sz.x + 5, 10);

        for (int i = 0; i < 10; ++i)
        {
            spSprite v = new Sprite;

            char name[32];
            safe_sprintf(name, "n%d", i);
            ResAnim* rs = res->getResAnim(name);

            v->setResAnim(rs);

            num->setActor(i, v);
        }
    }
}

void Counter::add()
{
    if (_running)
        return;

    _running = true;

    spNumber n = _numbers[0];
    while (n->getCurrent() == 9)
    {
        n = _numbers[n->getPos() + 1];
        n->add();
    }

    _numbers[0]->add()->addDoneCallback(CLOSURE(this, &Counter::added));
}

void Counter::added(Event* e)
{
    _running = false;

    _src++;
    if (_src == _dest)
        return;

    add();
}

void Counter::setValue(int v)
{
    if (v == _dest)
        return;

    _dest = v;
    add();
}

int  Counter::getValue() const
{
    return _dest;
}

void Counter::reset()
{
    _running = false;
    _src = 0;
    _dest = 0;

    for (int n = 0; n < NUM; ++n)
        _numbers[n]->reset();
}