#include "TreeInspectorLine.h"
#include "TreeInspectorPage.h"
#include "TreeInspectorPreview.h"
#include "TreeInspector.h"
#include "TextField.h"
#include "res/ResFont.h"
#include "res/Resources.h"
#include "Stage.h"
#include "DebugActor.h"

namespace oxygine
{

    TreeInspectorLine::TreeInspectorLine(): _page(0)
    {
        setTouchEnabled(false);
    }

    TreeInspectorLine::~TreeInspectorLine()
    {
    }


    void TreeInspectorLine::init(TreeInspectorPage* page, spActor item)
    {
        setSize(Vector2(0, 0));
        setCull(true);

        _page = page;
        _item = new TreeInspectorPreview();
        _item->init(item, Vector2(50, 50), false);
        addChild(_item);

        _itemTree = new TreeInspectorPreview();
        _itemTree->init(item, getStage()->getSize() / 2, true);


        spTextField tb = new TextField();
        tb->setTouchEnabled(false);
        tb->setColor(Color::Black);

        const float minWidth = 300.0f;

        TextStyle style;
        style.font = DebugActor::resSystem->getResFont("system");
        style.multiline = true;
        style.vAlign = TextStyle::VALIGN_TOP;
        tb->setStyle(style);
        tb->setSize(Vector2(minWidth, 0.0f));



        std::string desc = item->dump(0);

        tb->setHtmlText(desc);
        Vector2 ts = tb->getTextRect().size.cast<Vector2>();
        if (ts.x < minWidth)
            ts.x = minWidth;

        tb->setSize(ts);


        tb->setX(50.0f + 5);

        addChild(tb);

        setSize(0, 0);
        Vector2 sz = TreeInspector::calcBounds(this).size;
        setSize(sz);

        addTouchDownListener([ = ](Event*)
        {
            _itemTree->setPriority(_stage->getLastChild()->getPriority());
            _itemTree->setPosition(getStage()->getSize() / 2 - _itemTree->getSize() / 2);
            _stage->addChild(_itemTree);
        });

        addTouchUpListener([ = ](Event*)
        {
            _itemTree->detach();
        });
    }
}