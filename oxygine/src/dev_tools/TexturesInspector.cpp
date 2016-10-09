#include "TexturesInspector.h"
#include "core/NativeTexture.h"
#include "AnimationFrame.h"
#include "Sprite.h"
#include "initActor.h"
#include "SlidingActor.h"
#include "DebugActor.h"
#include "res/Resources.h"
#include "TextField.h"
#include "ColorRectSprite.h"
#include "utils/stringUtils.h"

namespace oxygine
{
    Vector2 fitSize(const Vector2& destSize, const Vector2& src);
    const Vector2 itemSize(256.0f, 256.0f);

    class TextureLine: public Box9Sprite
    {
    public:
        TextureLine(spNativeTexture t)
        {
            setVerticalMode(Box9Sprite::TILING_FULL);
            setHorizontalMode(Box9Sprite::TILING_FULL);
            Sprite::setResAnim(DebugActor::resSystem->getResAnim("checker"));

            AnimationFrame f;
            Vector2 s = fitSize(itemSize, Vector2((float)t->getWidth(), (float)t->getHeight()));

            setSize(s);

            Diffuse df;
            df.base = t;

            f.init(0, df, RectF(0, 0, 1.0f, 1.0f), RectF(0, 0, s.x, s.y), s);
            spSprite image = initActor(new Sprite,
                                       arg_blend = blend_disabled,
                                       arg_resAnim = f);
            addChild(image);

            spColorRectSprite rect = initActor(new ColorRectSprite,
                                               arg_color = Color(Color::White, 255),
                                               arg_attachTo = this);

            rect->addTween(Sprite::TweenColor(Color(Color::White, 0)), 4000, -1, true);

            char path[255];
            path::normalize(t->getName().c_str(), path);

            char txt[255];
            safe_sprintf(txt, "%s\n<div c=\"FF0000\">%s</div>-<div c=\"0000ff\">%dx%d</div>\nid: %d",
                         path,
                         textureFormat2String(t->getFormat()),
                         t->getWidth(), t->getHeight(), t->getObjectID());

            spTextField text = initActor(new TextField,
                                         arg_color = Color::Black,
                                         arg_w = (float)itemSize.x,
                                         arg_vAlign = TextStyle::VALIGN_TOP,
                                         arg_hAlign = TextStyle::HALIGN_LEFT,
                                         arg_multiline = true,
                                         arg_attachTo = rect,
                                         arg_htmlText = txt
                                        );

            text->setBreakLongWords(true);

            rect->setSize(text->getTextRect().size.cast<Vector2>() + Vector2(2, 2));
            rect->setY((itemSize.y - rect->getHeight()) / 2.0f);
        }

    };

    TexturesInspector::TexturesInspector(const Vector2& size)
    {
        setSize(size);

        spSlidingActor slide = new SlidingActor;
        slide->setSize(size);
        addChild(slide);


        float offsetY = 0;

        std::vector<spNativeTexture> textures = NativeTexture::getCreatedTextures();


        spTextField text = initActor(new TextField,
                                     arg_color = Color::White,
                                     arg_pos = Vector2(1, 1),
                                     arg_w = itemSize.x * 3.0f,
                                     arg_h = 30.0f,
                                     arg_vAlign = TextStyle::VALIGN_TOP,
                                     arg_hAlign = TextStyle::HALIGN_LEFT,
                                     arg_multiline = true,
                                     arg_attachTo = slide
                                    );


        offsetY += text->getTextRect().getBottom() + 5;

        spActor content = new Actor;
        content->setX(2);


        int numX = (int)(size.x / itemSize.x);

        int n = 0;
        int mem = 0;
        for (std::vector<spNativeTexture>::reverse_iterator i = textures.rbegin(); i != textures.rend(); ++i)
        {
            spNativeTexture t = *i;
            TextureLine* line = new TextureLine(t);

            float x = (n % numX) * (itemSize.x + 5.0f);
            float y = (n / numX) * (itemSize.y + 5.0f);
            line->setX(x);
            line->setY(y + offsetY);
            content->addChild(line);
            ++n;

            if (t->getHandle())
            {
                TextureFormat fmt = t->getFormat();
                int ram = t->getWidth() * t->getHeight();
                if (isCompressedFormat(fmt))
                {
                    switch (fmt)
                    {
                        case TF_PVRTC_4RGBA:
                            ram /= 2;
                            break;
                        case TF_ETC1:
                            ram /= 2;
                            break;
                        default:
                            break;
                    }
                }
                else
                    ram *= getBytesPerPixel(fmt);

                mem += ram;
            }
        }

        char txt[255];
        safe_sprintf(txt, "created textures: %d, vram: %d kb", textures.size(), mem / 1024);

        text->setText(txt);
        if (numX > n)
            numX = n;

        content->setSize(
            (itemSize.x + 5.0f) * numX,
            (itemSize.y + 5.0f) * (n + numX - 1.0f) / numX + offsetY);

        setWidth(content->getWidth());
        slide->setWidth(content->getWidth());

        //slide->setSize()
        slide->setContent(content);
    }

    TexturesInspector::~TexturesInspector()
    {

    }
}
