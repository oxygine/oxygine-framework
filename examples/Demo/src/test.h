#pragma once

#include "oxygine-framework.h"

using namespace oxygine;
using namespace std;


spTextField createText(const std::string& txt);
spButton createButtonHelper(spButton, const std::string& txt, EventCallback cb);

DECLARE_SMART(Test, spTest);
class Test: public Actor
{
public:
    Test();
    ~Test();

    static void init();
    static void free();

    static spTest instance;
    static Resources _resources;

    struct toggle
    {
        string text;
        int value;
        const void* data;
        toggle() {}
        toggle(const char* text_, int v_ = 0, const void* data_ = 0): text(text_), value(v_), data(data_) {}
    };

    spButton addButton(string id, string txt);
    void addToggle(string id, const toggle* t, int num);
    void updateText(string id, string txt);
    virtual void clicked(string id) {}
    virtual void toggleClicked(string id, const toggle* data) {}


    void notify(string text, int time = 400);

protected:
    void notifyDone(Event* ev);
    void _clicked(Event* event);
    void _toggleClicked(Event* event);
    void _back(Event* event);

    Color _color;
    Color _txtColor;

    spActor _content;
    spActor _ui;

    float _x;
    float _y;

    enum {MAX_NOTIFIES = 8};
    int _notifies[MAX_NOTIFIES];
};