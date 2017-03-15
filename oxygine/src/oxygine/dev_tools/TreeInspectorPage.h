#pragma once
#include "oxygine-include.h"
#include "Actor.h"

namespace oxygine
{
    DECLARE_SMART(TreeInspectorPage, spTreeInspectorPage);
    DECLARE_SMART(TreeInspectorLine, spTreeInspectorLine);

    class TreeInspector;
    class TreeInspectorPage: public Actor
    {
    public:
        TreeInspectorPage(TreeInspector* tree, TreeInspectorPage* parentPage);
        ~TreeInspectorPage();

        void init(spActor item);

        TreeInspector* getTreeInspector() {return _tree;}

    protected:
        void updateSize();
        void updateInternalSize();
        void updateChildrenPositions();

        Vector2 updateSizesNew();
        friend class TreeInspector;

    private:
        void _onMinMaxClick(Event*);

        spTreeInspectorLine _line;
        TreeInspector* _tree;
        TreeInspectorPage* _parentPage;
        spActor _pages;

    };
}