#pragma once
#include "../oxygine-include.h"
#include "../actor/Actor.h"

namespace oxygine
{
    class TreeInspectorPage;
    DECLARE_SMART(TreeInspectorPreview, spTreeInspectorPreview);

    class TreeInspectorLine: public Actor
    {
    public:
        TreeInspectorLine();
        ~TreeInspectorLine();

        void init(TreeInspectorPage* page, spActor item);

    protected:


    private:
        TreeInspectorPage* _page;
        spTreeInspectorPreview _item;
        spTreeInspectorPreview _itemTree;


        void _onMouseOver(bool over);
        void _onPress(bool over);
    };
}