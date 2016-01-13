#pragma once
#include "test.h"

class TestRender2Texture: public Test
{
public:
    spNativeTexture texture;
    Color color;
    spSprite preview;

    TestRender2Texture()
    {
        notify("touch to draw", 100000);

        color = Color(255, 255, 255, 32);

        Vector2 size = content->getSize();

        texture = IVideoDriver::instance->createTexture();
        texture->init((int)size.x, (int)size.y, TF_R8G8B8A8, true);

        preview = new Sprite;
        preview->attachTo(content);


        AnimationFrame frame;
        Diffuse df;
        df.base = texture;
        frame.init(0, df,
                   RectF(0, 0, size.x / texture->getWidth(), size.y / texture->getHeight()),
                   RectF(Vector2(0, 0), size), size);
        preview->setAnimFrame(frame);
        preview->setBlendMode(blend_disabled);

        content->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestRender2Texture::onMove));
        content->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &TestRender2Texture::onDown));
    }

    void onDown(Event* ev)
    {
        color = Color(rand() % 255, rand() % 255, rand() % 255, 32);
        onMove(ev);
    }

    void onMove(Event* ev)
    {
        TouchEvent* te = (TouchEvent*)ev;
        if (!te->getPointer()->isPressed())
            return;

        STDRenderer renderer(0);

        Rect viewport(Point(0, 0), content->getSize().cast<Point>());
        renderer.initCoordinateSystem(viewport.getWidth(), viewport.getHeight(), true);

        IVideoDriver* driver = IVideoDriver::instance;

        driver->setRenderTarget(texture);
        driver->setViewport(viewport);

        RectF destRect(te->localPosition - Vector2(16, 16), Vector2(32, 32));

        //begin rendering
        renderer.begin(0);

        ResAnim* brush = resources.getResAnim("brush");
        AnimationFrame frame = brush->getFrame(0);
        const Diffuse& df = frame.getDiffuse();
        renderer.setTexture(df.base, 0);
        renderer.setBlendMode(blend_alpha);
        renderer.draw(color, frame.getSrcRect(), destRect);
        renderer.end();

        //restore to default render target
        driver->setRenderTarget(0);
    }
};
