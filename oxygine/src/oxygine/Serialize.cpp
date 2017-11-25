#include "Serialize.h"
#include "actor/Box9Sprite.h"
#include "actor/ClipRectActor.h"
#include "actor/ColorRectSprite.h"
#include "actor/MaskedSprite.h"
#include "actor/Polygon.h"
#include "actor/ProgressBar.h"
#include "actor/SlidingActor.h"
#include "actor/Sprite.h"
#include "actor/TextField.h"
#include "actor/WebImage.h"

namespace oxygine
{
    //#define S(s) #s
    //#define SS(s) ##s
#define CREATE(T) if (!strcmp(#T, type)) return new T

    spActor creator::create(const char* type) const
    {
        CREATE(Sprite);
        CREATE(TextField);
        CREATE(Actor);
        CREATE(ProgressBar);
        CREATE(Box9Sprite);
        CREATE(SlidingActor);
        CREATE(ColorRectSprite);
        CREATE(ClipRectActor);
        CREATE(Polygon);
        CREATE(MaskedSprite);
        CREATE(WebImage);


        return 0;
    }

    static void link(pugi::xml_node node, spActor actor, spActor root)
    {
        deserializeLinkData data;
        data.node = node;
        data.root = root;
        actor->deserializeLink(&data);

        spActor child = actor->getFirstChild();
        node = node.first_child();
        while (child)
        {
            link(node, child, root);
            node = node.next_sibling();
            child = child->getNextSibling();
        }
    }

    void deserializeLinkData::link(pugi::xml_node node, spActor actor)
    {
        oxygine::link(node, actor, actor);
    }

    spActor deserializedata::deser(pugi::xml_node node, const creator* factory)
    {
        deserializedata d;
        d.node = node;
        d.factory = factory;
        const char* name = node.name();
        spActor actor = factory->create(name);
        actor->deserialize(&d);

        return actor;
    }
}
