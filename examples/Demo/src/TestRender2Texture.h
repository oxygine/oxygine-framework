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
        color = Color(rand() % 255, rand() % 255, rand() % 255, 128);
        onMove(ev);
    }

    void onMove(Event* ev)
    {
        TouchEvent* te = (TouchEvent*)ev;
        if (!te->getPointer()->isPressed())
            return;

        STDRenderer& renderer = *STDRenderer::instance;
        IVideoDriver* driver = IVideoDriver::instance;


        Rect viewport(Point(0, 0), content->getSize().cast<Point>());
        renderer.initCoordinateSystem(viewport.getWidth(), viewport.getHeight(), true);

        driver->setRenderTarget(texture);
        driver->setViewport(viewport);

#if 0
        renderer.begin(0);
        RectF destRect(te->localPosition - Vector2(16, 16), Vector2(32, 32));

        ResAnim* brush = resources.getResAnim("brush");
        AnimationFrame frame = brush->getFrame(0);
        const Diffuse& df = frame.getDiffuse();
        renderer.setTexture(df.base, 0, true);
        renderer.setBlendMode(blend_premultiplied_alpha);
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
        driver->setRenderTarget(0);
    }
};
