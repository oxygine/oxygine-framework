#pragma once
#include "Actor.h"
#include "TextField.h"
#include "Button.h"
#include "jewels.h"

#define FIELD_SIZE 6



using namespace oxygine;

DECLARE_SMART(GameFrame, spGameFrame);
DECLARE_SMART(Field, spTField);
DECLARE_SMART(TFieldSolutions, spTFieldSolutions);

typedef enum {fsWaiting, fsSwaping, fsFalling, fsChecking, fsAutoSwaping, fsAnimation} TFieldState;

class TFieldSolutions
{
public:
    Point target;
    Point jewel;

    int jewel_count;

    TFieldSolutions(Point Target, Point Jewel, int count)
    {
        target = Target;
        jewel = Jewel;
        jewel_count = count;
    }

};

class TFieldVariants
{
public:
    Point index;
    int points;
    TFieldVariants(Point Index, int score) {index = Index; points = score;}
};


class Field : public Actor
{
public:
    TFieldState getState() {return state;}

    spJewel jewels[FIELD_SIZE][FIELD_SIZE];

    Field();
    spJewel At(Point index);

    Vector2 getCellPosition(int i, int j);
    Point getCellIndex(Vector2 position);

    void JewelClick(Event* event);
    bool Swap(spJewel First, spJewel Second, bool skip_animation = false);
    bool BackSwap();

private:
    TFieldState state;
    Vector2 size;

    Point jewel_drag_ind;
    Point jewel_new_ind;
    int droped_count;
    int animated_count;

    int goUp(Point ind, bool scoreit = false);
    int goDown(Point ind, bool scoreit = false);
    int goRight(Point ind, bool scoreit = false);
    int goLeft(Point ind, bool scoreit = false);
    int CheckJewel(Point ind, bool donotscore = false);

    void EndSwapCallback(Event* ev);
    void EndAnimationCallback(Event* ev);
    void DropEndCallback(Event* ev);

    bool back_swap;
    bool DropField();
    void DropJewel(spJewel Target, spJewel Jewel);
    void ForceSwap(spJewel First, spJewel Second);
    int FindFreeCell(int column, int limit = 0);
    int StartAnimation(Point ind);
    int GenerateNewJewels(int column);
    bool CheckField();
    void CleanField();
    void FillField(bool first_time = false);
    bool FindSolutions();
    void RefreshField();
};



class GameFrame : public Sprite
{
public:
    GameFrame();

};
