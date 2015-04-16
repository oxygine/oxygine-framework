#pragma once
#include "oxygine-framework.h"
#include <vector>
using namespace oxygine;
using namespace std;

DECLARE_SMART(Scene, spScene);
class Scene: public EventDispatcher
{
public:
    Scene();

    //declare own Event type
    //it would be fired when scene would completely hidden
    class HiddenEvent: public Event
    {
    public:
        //define unique int ID with makefourcc 'SHid' = SceneHidden
        enum {EVENT = makefourcc('S', 'H', 'i', 'd')};

        HiddenEvent(Scene* scene_): Event(EVENT), scene(scene_) {};
        Scene* scene;
    };

    void show();

    //fires HiddenEvent when scene completed hidden
    void hide();

    void changeScene(spScene next);
    spActor getView() const {return _view;}

protected:
    virtual void _show() {}
    virtual void _hide() {}

    void hidden(Event* ev);
    spActor _view;
};



