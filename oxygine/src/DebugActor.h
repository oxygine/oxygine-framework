#pragma once
#include "oxygine-include.h"
#include "Actor.h"


namespace oxygine
{
    DECLARE_SMART(DebugActor, spDebugActor);

    /**DebugActor is helper class. It displays FPS, memory usage, other stats and dev tools buttons*/
    class DebugActor: public Actor
    {
    public:
        static spDebugActor instance;

        static Resources* resSystem;
        static void initialize();
        static void show();
        static void toggle();
        static void hide();
        static void release();
        /**where to display DebugActor. 0 - top left, 1 - top right, 2 - bottom right, 3 - bottom left corner*/
        static void setCorner(int corner);
        static void addDebugString(const char* format, ...);
        static std::string getDefaultName() { return "debug_actor"; }

        DebugActor();
        ~DebugActor();


        /**function for debug. Helps you to find actor who handled TouchEvent*/
        void showTouchedActor(bool show);
        void showTexel2PixelErrors(bool show);

    protected:
        void onAdded2Stage() override;
        void onRemovedFromStage() override;
        void showDevMenu(spActor data);
        void _btnClicked(Event* ev);


        void doUpdate(const UpdateState& us) override;
        void render(const RenderState&) override;
        void addButton(float& x, const char* name, const char* anim);
    private:

        spColorRectSprite _bg;
        spTextField _text;
        std::string _debugText;
        int _frames;
        timeMS _startTime;


        bool _showTouchedActor;
        bool _showTexel2PixelErrors;

        bool _dragging;
        Vector2 _local;

        void onEvent(Event* ev);
        void onDAEvent(Event* ev);
    };
}