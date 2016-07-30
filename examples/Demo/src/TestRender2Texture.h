#pragma once
#include "test.h"
#include "STDMaterial.h"

class TestRender2Texture : public Test
{
public:
    spNativeTexture texture;
    Color color;
    spSprite preview;

    TestRender2Texture()
    {
        notify("touch to draw", 100000);


        spSprite back = new Sprite;
        back->setResAnim(resources.getResAnim("bg"));
        back->attachTo(content);
        back->setPosition(content->getSize() / 2 - back->getSize() / 2);



        color = Color(255, 255, 255, 32);

        Vector2 size = content->getSize();

        texture = IVideoDriver::instance->createTexture();
        texture->init((int)size.x, (int)size.y, TF_R8G8B8A8, true);

        preview = new Sprite;
        preview->attachTo(content);


        AnimationFrame frame;
        Diffuse df;
        df.base = texture;
        df.premultiplied = false;
        frame.init(0, df,
                   RectF(0, 0, size.x / texture->getWidth(), size.y / texture->getHeight()),
                   RectF(Vector2(0, 0), size), size);
        preview->setAnimFrame(frame);


        content->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestRender2Texture::onMove));
        content->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &TestRender2Texture::onDown));

    }

    void onDown(Event* ev)
    {
        color = Color(rand() % 255, rand() % 255, rand() % 255, 128);
        onMove(ev);
    }

    void onMove(Event* ev)
    {
        TouchEvent* te = (TouchEvent*)ev;
        bool left = te->getPointer()->isPressed();
        bool right = te->getPointer()->isPressed(MouseButton_Right);
        if (!left && !right)
            return;

        STDRenderer& renderer = *STDRenderer::instance;
        IVideoDriver* driver = IVideoDriver::instance;


        Rect viewport(Point(0, 0), content->getSize().cast<Point>());
        renderer.initCoordinateSystem(viewport.getWidth(), viewport.getHeight(), true);

        spNativeTexture previousRT = driver->getRenderTarget();

        driver->setRenderTarget(texture);
        driver->setViewport(viewport);

#if 1
        renderer.begin(0);
        RectF destRect(te->localPosition - Vector2(16, 16), Vector2(32, 32));

        ResAnim* brush = resources.getResAnim(left ? "brush" : "brush_eraser");
        AnimationFrame frame = brush->getFrame(0);
        const Diffuse& df = frame.getDiffuse();
        renderer.setTexture(df.base, 0);
        if (left)
            renderer.setBlendMode(blend_premultiplied_alpha);
        else
        {
            color = Color::White;
            renderer.setBlendMode(blend_erase);
        }


        renderer.draw(color, frame.getSrcRect(), destRect);
        renderer.end();
#else
        //how to render actors to texture
        Material::setCurrent(0);

        spActor actor = new ManageResTest;
        actor->setScale(0.5f);

        RenderState rs;
        rs.material = STDMaterial::instance;
        actor->setAlpha(64);
        actor->render(rs);
        Material::setCurrent(0);
#endif

        //restore to default render target
        driver->setRenderTarget(previousRT);
    }
};
