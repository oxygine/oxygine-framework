#include "Board.h"
#include "Jewel.h"

Board::Board(): _size(0, 0), _selected(0)
{

}

Vector2 getPos(const Point& p)
{
    Vector2 pos;
    pos.x = p.x * JewelSize.x;
    pos.y = p.y * JewelSize.y;
    pos += JewelSize / 2;

    return pos;
}

void Board::init(int w, int h)
{
    _size.x = w;
    _size.y = h;

    _view = new Sprite;

    _field.resize(_size.x * _size.y);

    for (int y = 0; y < _size.y; ++y)
    {
        for (int x = 0; x < _size.x; ++x)
        {
            _field[x + y * _size.x].pos = Point(x, y);
        }
    }

    _view->setSize(_size.x * JewelSize.x, _size.y * JewelSize.y);

    _view->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &Board::touched));
    _view->setInputChildrenEnabled(false);

    _view->setCallbackDoUpdate(CLOSURE(this, &Board::update));
}

struct swapData: public Object
{
    swapData(space* A, space* B): a(A), b(B) {}
    space* a;
    space* b;
};

spTween Board::swap(space& a, space& b)
{
    spTween tween;
    tween = a.jewel->swap(getPos(b.pos));
    tween = b.jewel->swap(getPos(a.pos));
    std::swap(a.jewel, b.jewel);


    tween->setDataObject(new swapData(&a, &b));
    return tween;
}


void Board::swapped(Event* event)
{
    TweenEvent* te = safeCast<TweenEvent*>(event);
    spObject data = te->tween->getDataObject();
    swapData* sw = safeCast<swapData*>(data.get());
    space& a = *sw->a;
    space& b = *sw->b;
    std::vector<space*> spaces;
    findMatches(spaces, a);
    findMatches(spaces, b);
    if (spaces.empty())
    {
        a.jewel->swap(getPos(b.pos));
        b.jewel->swap(getPos(a.pos));

        std::swap(a.jewel, b.jewel);
    }
}

space* Board::getSpace(const Point& pos, bool check)
{
    if (pos.x < 0 || pos.x >= _size.x)
        return 0;
    if (pos.y < 0 || pos.y >= _size.y)
        return 0;

    space& sp = _field[pos.x + pos.y * _size.x];
    if (check)
    {
        if (!sp.jewel)
            return 0;
        if (sp.jewel->isExploding())
            return 0;
        if (sp.jewel->isSwaping())
            return 0;
        if (sp.jewel->isDead())
            return 0;
        if (sp.jewel->isFalling())
            return 0;
    }
    return  &sp;
}

const Point dirUp(0, -1);
const Point dirDown(0, 1);
const Point dirLeft(-1, 0);
const Point dirRight(1, 0);

void Board::findMatches(std::vector<space*>& spaces, space& sp)
{
    std::vector<space*> hor;
    findMatches(hor, sp, dirUp);
    findMatches(hor, sp, dirDown);

    std::vector<space*> ver;
    findMatches(ver, sp, dirLeft);
    findMatches(ver, sp, dirRight);

    if (hor.size() >= 2)
    {
        spaces.insert(spaces.end(), hor.begin(), hor.end());
    }

    if (ver.size() >= 2)
    {
        spaces.insert(spaces.end(), ver.begin(), ver.end());
    }

    if (!spaces.empty())
        spaces.push_back(&sp);
}

void Board::findMatches(std::vector<space*>& spaces, space& sp, const Point& dir)
{
    Point pos = sp.pos;
    while (true)
    {
        pos += dir;
        space* s = getSpace(pos);
        if (!s)
            return;
        if (sp.jewel->getType() != s->jewel->getType())
            return;

        spaces.push_back(s);
    }
}

void Board::update(const UpdateState& us)
{
    for (int y = 0; y < _size.y; ++y)
    {
        for (int x = 0; x < _size.x; ++x)
        {
            space* sp = getSpace(Point(x, y));
            if (!sp)
                continue;

            std::vector<space*> spaces;
            findMatches(spaces, *sp);

            for (size_t i = 0; i < spaces.size(); ++i)
            {
                space* s = spaces[i];
                s->jewel->explode();
            }

        }
    }

    for (int y = 0; y < _size.y; ++y)
    {
        for (int x = 0; x < _size.x; ++x)
        {
            space& sp = _field[x + y * _size.x];

            spJewel jewel = sp.jewel;
            if (!jewel)
                continue;

            if (jewel->isDead())
                sp.jewel = 0;
        }
    }


    for (int x = 0; x < _size.x; ++x)
    {
        float last_y = 0;
        for (int y = _size.y - 1; y >= 0; --y)
        {
            space& sp = _field[x + y * _size.x];

            if (!sp.jewel)
            {
                spJewel fallJewel;
                for (int ty = y - 1; ty >= 0; --ty)
                {
                    space* fall = getSpace(Point(x, ty), false);
                    if (!fall->jewel)
                        continue;

                    fallJewel = fall->jewel;
                    fall->jewel = 0;
                    break;
                }


                Vector2 newPos = getPos(sp.pos);

                if (fallJewel)
                {
                    OX_ASSERT(fallJewel->isDead() == false);
                }
                else
                {
                    last_y -= JewelSize.y + 10 + rand() % 65;

                    fallJewel = new Jewel;
                    spActor view = fallJewel->getView();

                    view->setPosition(Vector2(newPos.x, last_y));
                    view->attachTo(_view);

                }

                fallJewel->fall(getPos(Point(x, y)));
                sp.jewel = fallJewel;
            }
        }
    }
}

void Board::touched(Event* event)
{
    TouchEvent* te = safeCast<TouchEvent*>(event);
    Vector2 pos = te->localPosition;
    //log::messageln("%d - %d: %.2f, %.2f", event->target->getObjectID(), event->currentTarget->getObjectID(), pos.x, pos.y);
    Point spacePos;
    spacePos.x = (int)(pos.x / JewelSize.x);
    spacePos.y = (int)(pos.y / JewelSize.y);

    space* sp = getSpace(spacePos);


    if (_selected)
    {
        _selected->jewel->unselect();
        if (sp)
        {
            Point dir = _selected->pos - sp->pos;
            if (dir.x == 0 && abs(dir.y) == 1 ||
                    dir.y == 0 && abs(dir.x) == 1)
            {
                spTween tween = swap(*_selected, *sp);
                tween->setDoneCallback(CLOSURE(this, &Board::swapped));
            }
        }

        _selected = 0;
    }
    else
    {
        _selected = sp;
        if (sp)
            sp->jewel->select();
    }
}

spActor Board::getView()
{
    return _view;
}

void Board::free()
{
    _field.clear();
    _view->detach();
    _view = 0;
    _current = 0;
}