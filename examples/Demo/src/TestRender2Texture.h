#pragma once
#include "test.h"
#include "STDMaterial.h"
#include "core/gl/oxgl.h"

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
        frame.init(0, df,
                   RectF(0, 0, size.x / texture->getWidth(), size.y / texture->getHeight()),
                   RectF(Vector2(0, 0), size), size);
        preview->setAnimFrame(frame);


        content->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestRender2Texture::onMove));
        content->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &TestRender2Texture::onDown));

    }

    void onDown(Event* ev)
    {
        color = Color(rand() % 255, rand() % 255, rand() % 255, 64);
        onMove(ev);
    }

    void onMove(Event* ev)
    {
        TouchEvent* te = (TouchEvent*)ev;
        bool left = te->getPointer()->isPressed();
        bool right = te->getPointer()->isPressed(MouseButton_Right);
        if (!left && !right)
            return;

        STDRenderer& renderer = *STDRenderer::getCurrent();



#if 1
        renderer.begin(texture);
        RectF destRect(te->localPosition - Vector2(30, 30), Vector2(60, 60));

        ResAnim* brush = resources.getResAnim("brush");
        AnimationFrame frame = brush->getFrame(0);
        const Diffuse& df = frame.getDiffuse();
        renderer.setTexture(df.base);
        if (left)
            renderer.setBlendMode(blend_premultiplied_alpha);
        else
        {
            renderer.setBlendMode(blend_add);
            oxglBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        }

        renderer.draw(color, frame.getSrcRect(), destRect);
        renderer.end();
        if (!left)
            oxglBlendEquation(GL_FUNC_ADD);//restore to default value
#else

        renderer.begin(texture);
        spActor actor = new ManageResTest;
        actor->setScale(0.5f);

        RenderState rs;
        actor->setAlpha(64);
        actor->render(rs);
        renderer.end();
#endif
    }
};
