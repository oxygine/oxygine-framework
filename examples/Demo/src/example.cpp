#include "oxygine/oxygine-framework.h"

#include "test.h"
#include "TestPerf.h"
#include "TestTweens.h"
#include "TestDrag.h"
#include "TestManageRes.h"
#include "TestRender2Texture.h"
#include "TestText.h"
#include "TestTextureFormat.h"
#include "TestTexel2Pixel.h"
#include "TestSliding.h"
#include "TestProgressBar.h"
#include "TestBox9Sprite.h"
#include "TestClipRect.h"
#include "TestUserShader.h"
#include "TestUserShader2.h"
#include "TestMask.h"
#include "TestPolygon.h"
#include "TestInputText.h"
#include "TestHttp.h"
#include "TestAlphaHitTest.h"
#include "TestCounter.h"
#include "TestTweenText.h"
#include "TestTweenShine.h"
#include "TestTouches.h"
#include "TestColorFont.h"
#include "TestSignedDistanceFont.h"
#include "TestTweenPostProcessing.h"
#include "TestEdges.h"
#include "TestCamera.h"
#include "TestTiled.h"
#include "TestOpenGL.h"

#ifdef __S3E__
#include "s3eKeyboard.h"
#endif

using namespace oxygine;


//This contains our resources
//In a real project you would have more than one Resources declaration.
//It is important on mobile devices with limited memory and you would load/unload them
Resources resources;

//#define MULTIWINDOW 1

#if MULTIWINDOW
spStage stage2;
#endif

class TestActor: public Test
{
public:

    TestActor()
    {

        _x = 90;//getStage()->getWidth()/2.0f;
        _y = 80;


        addButton("tweens", "Tweens");
        addButton("text", "Text");
        addButton("progress_bar", "Progress Bar");
        addButton("drag", "Drag and Drop with Test Intersections");
        addButton("drag2", "Drag and Drop2");
        addButton("hittest", "Alpha Hit Test");
        addButton("perf", "Performance");
        addButton("manage_res", "Manage Resources");
        addButton("texture_format", "Textures Format");
        addButton("r2t", "Render to Texture");
        addButton("t2p", "Texel to Pixel");
        addButton("edges", "Edges");
        addButton("touches", "Touches");
        addButton("sliding", "Sliding Actor");
        addButton("box9sprite", "Box9 Sprite");
        addButton("cliprect", "ClipRect Actor");
        addButton("usershader", "Extended UberShader");
        addButton("usershader2", "Custom shaders and render");
        addButton("opengl", "OpenGL usage");
        addButton("multicolorfont", "Outer Font Color");
        addButton("sdf", "Signed Distance Font");
        addButton("mask", "Mask");
        addButton("polygon", "Polygon");
        addButton("inputtext", "Input Text");
        addButton("openbrowser", "Open Browser");
        addButton("http", "Http requests");
        addButton("tweenpp", "Post Processing Tweens");

        _color = Color::Red;
        _txtColor = Color::White;

        _y = 5;
        _x += 200;
        addButton("counter", "Counter");
        addButton("tweentext", "Tween Text");
        addButton("tweenshine", "Tween Shine");
        addButton("mtz", "MultiTouch Camera/Zoom");
        addButton("tiled", "Tiled Map Editor");

        //clicked("tiled");
    }

    void showTest(spActor actor)
    {
        spStage stage = getStage();
#if MULTIWINDOW
        stage = stage2;
#else
        setVisible(false);
#endif
        stage->addChild(actor);
    }

    void clicked(string id)
    {
        if (id == "perf") showTest(new PerfTest);
        if (id == "tweens") showTest(new TweensTest);
        if (id == "drag") showTest(new DragTest);
        if (id == "drag2") showTest(new Drag2Test);
        if (id == "hittest") showTest(new TestAlphaHitTest);
        if (id == "manage_res") showTest(new ManageResTest);
        if (id == "r2t") showTest(new TestRender2Texture);
        if (id == "text") showTest(new TestText);
        if (id == "progress_bar") showTest(new TestProgressBar);
        if (id == "texture_format") showTest(new TestTextureFormat);
        if (id == "sliding") showTest(new TestSliding);
        if (id == "t2p") showTest(new TestTexel2Pixel);
        if (id == "touches") showTest(new TestTouches);
        if (id == "box9sprite") showTest(new TestBox9Sprite);
        if (id == "cliprect") showTest(new TestClipRect);
        if (id == "usershader") showTest(new TestUserShader);
        if (id == "usershader2") showTest(new TestUserShader2);
        if (id == "opengl") showTest(new TestOpenGL);
        if (id == "mask") showTest(new TestMask);
        if (id == "polygon") showTest(new TestPolygon);
        if (id == "inputtext") showTest(new TestInputText);
        if (id == "http") showTest(new TestHttp);
        if (id == "counter") showTest(new TestCounter);
        if (id == "tweentext") showTest(new TestTweenText);
        if (id == "tweenshine") showTest(new TestTweenShine);
        if (id == "multicolorfont") showTest(new TestColorFont);
        if (id == "sdf") showTest(new TestSignedDistanceFont);
        if (id == "tweenpp") showTest(new TestTweenPostProcessing);
        if (id == "edges") showTest(new TestEdges);
        if (id == "mtz") showTest(new TestCamera);
        if (id == "tiled") showTest(new TestTiled);
        if (id == "openbrowser")
        {
            core::execute("http://oxygine.org/");
            setVisible(true);
        }
    }
};

void example_preinit()
{
}


void example_init()
{
    //Load resources in xml file
    resources.loadXML("xmls/res.xml");
    Test::init();

    Test::instance = new TestActor;
    getStage()->addChild(Test::instance);

    //Initialize http requests
    HttpRequestTask::init();


#if MULTIWINDOW
    SDL_Window* window2 = SDL_CreateWindow("Second Oxygine Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, getStage()->getWidth(), getStage()->getHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    stage2 = new Stage(false);
    stage2->setSize(getStage()->getSize());
    stage2->associateWithWindow(window2);
#endif
}

void example_update()
{
#if MULTIWINDOW
    stage2->update();
    SDL_Window* wnd = stage2->getAssociatedWindow();
    if (core::beginRendering(wnd))
    {
        Color clearColor(32, 32, 32, 255);
        Rect viewport(Point(0, 0), core::getDisplaySize());
        //render all actors. Actor::render would be called also for all children
        stage2->render(clearColor, viewport);

        core::swapDisplayBuffers(wnd);
    }
#endif
}

void example_destroy()
{
    resources.free();
    Test::free();
    HttpRequestTask::release();
}