#pragma once
#include "../oxygine-include.h"
#include "../actor/ClipRectActor.h"
#include "../actor/ColorRectSprite.h"
#include "../Draggable.h"

namespace oxygine
{
    DECLARE_SMART(DeveloperMenu, spDeveloperMenu);
    class Resources;

    class DeveloperMenu: public ColorRectSprite
    {
    public:
        static std::string getDefaultName() { return "tree_inspector"; }

        DeveloperMenu();
        ~DeveloperMenu();

        void init(const Vector2& size, const char* text, spActor data, const Color& color);

        Vector2 getBodySize() const;

    private:
        void close(Event* ev);

        Resources* _resSystem;

        spActor _body;
        Draggable _topDrag;
    };
}