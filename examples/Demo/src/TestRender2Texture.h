#pragma once
#include "test.h"

class TestRender2Texture : public Test
{
public:
    spNativeTexture texture;
    Color color;
    spSprite preview;

    bool painter;

    TestRender2Texture()
    {
        notify("touch to draw", 100000);


        spSprite back = new Sprite;
        back->setResAnim(resources.getResAnim("bg"));
        back->attachTo(_content);
        back->setSize(getStage()->getSize());

        back->setPosition(_content->getSize() / 2 - back->getScaledSize() / 2);



        color = Color(255, 255, 255, 32);

        Vector2 size = _content->getSize();

        texture = IVideoDriver::instance->createTexture();
        texture->init((int)size.x, (int)size.y, TF_R8G8B8A8, true);

        preview = new Sprite;
        preview->attachTo(_content);


        AnimationFrame frame;
        Diffuse df;
        df.base = texture;
        frame.init(0, df,
                   RectF(0, 0, size.x / texture->getWidth(), size.y / texture->getHeight()),
                   RectF(Vector2(0, 0), size), size);
        preview->setAnimFrame(frame);


        _content->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestRender2Texture::onMove));
        _content->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &TestRender2Texture::onDown));


        Test::toggle t[2];
        t[0].text = "eraser";
        t[1].text = "paint";
        addToggle("mode", t, 2);

        painter = true;
    }

    void toggleClicked(string id, const toggle* t) override
    {
        painter = !painter;
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

        STDRenderer& renderer = *STDRenderer::getCurrent();

#if 1
        bool paint = painter && !right;
        renderer.begin(texture);

        Vector2 size = getStage()->getSize() / 20;
        RectF destRect(te->localPosition - size, size * 2);

        ResAnim* brush = resources.getResAnim("brush");
        AnimationFrame frame = brush->getFrame(0);
        const Diffuse& df = frame.getDiffuse();
        rsCache().setTexture(0, df.base);
        if (paint)
            rsCache().setBlendMode(blend_premultiplied_alpha);
        else
        {
            rsCache().setBlendMode(blend_add);
            oxglBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        }

        renderer.addQuad(color, frame.getSrcRect(), destRect);
        renderer.end();
        if (!paint)
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
