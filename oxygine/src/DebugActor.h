#pragma once
#include "oxygine_include.h"
#include "Actor.h"


namespace oxygine
{
    class ResFontBM;
    class ResAnim;
    class Resources;

    DECLARE_SMART(TextField, spTextField);
    DECLARE_SMART(ColorRectSprite, spColorRectSprite);
    DECLARE_SMART(DebugActor, spDebugActor);

    /**DebugActor is helper class. It displays FPS, memory usage, other stats and dev tools buttons*/
    class DebugActor: public Actor
    {
    public:
        static spDebugActor instance;

        static Resources* resSystem;
        static void initialize();
        static void show();
        static void release();
        static std::string getDefaultName() { return "debug_actor"; }

        DebugActor();
        ~DebugActor();

        /**where to display DebugActor. 0 - top left, 1 - top right, 2 - bottom right, 3 - bottom left corner*/
        void setCornerPosition(int corner);
        //void addDebugString(const string &str);
        void addDebugString(const char* format, ...);

        /**function for debug. Helps you to find actor who handled TouchEvent*/
        void showTouchedActor(bool show);
        void showTexel2PixelErrors(bool show);

    protected:
        void onAdded2Stage();
        void onRemovedFromStage();
        void showDevMenu(spActor data);
        void _btnClicked(Event* ev);


        void doUpdate(const UpdateState& us);
        void render(RenderState const& parentRenderState);
        void addButton(float& x, const char* name, const char* anim);
    private:

        spColorRectSprite _bg;
        spTextField _text;
        std::string _debugText;
        int _frames;
        timeMS _startTime;
        int _corner;

        bool _showTouchedActor;
        bool _showTexel2PixelErrors;

        void onEvent(Event* ev);
        void onDAEvent(Event* ev);
    };
}