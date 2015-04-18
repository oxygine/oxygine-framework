#pragma once
#include <vector>
#include "oxygine-framework.h"
#include <vector>
#include "Jewel.h"

using namespace oxygine;

DECLARE_SMART(Jewel, spJewel);

struct space
{
    Point pos;
    spJewel jewel;
};

class Board
{
public:
    Board();

    void init(int w, int h);

    spActor getView();


private:

    space* getSpace(const Point& pos, bool check = true);

    void update(const UpdateState& us);

    spTween swap(space& a, space& b);
    void touched(Event*);
    void swapped(Event*);

    void findMatches(std::vector<space*>& spaces, space& sp);
    void findMatches(std::vector<space*>& spaces, space& sp, const Point& dir);

    spSprite            _view;
    space*              _selected;
    spTween             _current;
    std::vector<space>  _field;
    Point               _size;
};