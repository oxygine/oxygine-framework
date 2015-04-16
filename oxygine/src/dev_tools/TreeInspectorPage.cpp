#include "TreeInspectorPage.h"
#include "TreeInspectorLine.h"
#include "TreeInspector.h"
#include "Button.h"
#include "res/Resources.h"


namespace oxygine
{
    TreeInspectorPage::TreeInspectorPage(TreeInspector* tree, TreeInspectorPage* parentPage): _tree(tree), _parentPage(parentPage)
    {
        //setTouchEnabled(false);
    }

    TreeInspectorPage::~TreeInspectorPage()
    {
        /*
        if (_pages)
            _pages->detach();
        _pages = 0;

        removeChildren();
        */
    }

    void TreeInspectorPage::init(spActor item)
    {
        OX_ASSERT(item);


        setSize(Vector2(0, 0));
        _line = new TreeInspectorLine();
        _line->setPosition(Vector2(5, 5));
        _line->init(this, item);
        addChild(_line);

        float y = _line->getHeight() + _line->getY() + 5.0f;

        if (item->getFirstChild())
        {
            spButton button;


            if (_parentPage)// && item->getFirstChild() != item->getLastChild())
            {
                button = new Button();
                button->setResAnim(_tree->_resSystem->getResAnim("checkbox"));
                button->setPosition(Vector2(15.0f, y));
                button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TreeInspectorPage::_onMinMaxClick));

                addChild(button);
            }

            _pages = new Actor();
            _pages->setTouchEnabled(false);
            _pages->setX(-15);
            _pages->setY((float)y);


            spActor child = item->getFirstChild();
            while (child)
            {
                if (!child->isName("tree_inspector") && !child->isName("debug_actor"))
                {
                    spTreeInspectorPage page = new TreeInspectorPage(_tree, this);
                    page->init(child);
                    _pages->addChild(page);
                }

                child = child->getNextSibling();
            }

            if (item->getVisible())
            {
                addChild(_pages);
                if (button)
                    button->setRow(1);
            }
            else
            {
                if (button)
                    button->setRow(0);
            }
        }

        updateInternalSize();
    }

    void TreeInspectorPage::updateChildrenPositions()
    {
        if (_pages)
        {
            float y = 0;
            spActor child = _pages->getFirstChild();
            while (child)
            {
                child->setPosition(Vector2(50.0f, y));
                y += child->getHeight() + 5.0f;

                child = child->getNextSibling();
            }

            _pages->setSize(Vector2(0, 0));
            _pages->setSize(TreeInspector::calcBounds(_pages.get()).size);
        }
    }

    void TreeInspectorPage::updateInternalSize()
    {
        updateChildrenPositions();

        if (_pages && _pages->getParent())
        {


        }

        setSize(Vector2(0, 0));
        setSize(TreeInspector::calcBounds(this).size.cast<Vector2>() + Vector2(5, 5));
    }

    Vector2 TreeInspectorPage::updateSizesNew()
    {
        setSize(0, 0);
        setPosition(0, 0);

        if (_pages)
            _pages->setVisible(false);
        Vector2 ns = TreeInspector::calcBounds(this).size + Vector2(5, 0);
        if (_pages)
            _pages->setVisible(true);



        if (_pages && _pages->getParent())
        {
            spActor child = _pages->getFirstChild();
            float y = 0;
            float x = 0;
            while (child)
            {
                spTreeInspectorPage page = safeSpCast<TreeInspectorPage>(child);
                Vector2 pageSize = page->updateSizesNew();
                page->setY((float)y);
                y += pageSize.y;
                x = std::max(x + 50, pageSize.x);
                child = child->getNextSibling();
            }
            ns.y += y;
            ns.x = x;
        }


        setSize(ns);

        setX(50);
        return getSize();
    }

    void TreeInspectorPage::updateSize()
    {
        updateInternalSize();

        if (_parentPage)
            _parentPage->updateSize();
    }

    void TreeInspectorPage::_onMinMaxClick(Event* ev)
    {
        Button* b = safeCast<Button*>(ev->currentTarget.get());
        b->setRow(b->getRow() == 1 ? 0 : 1);
        if (_pages->getParent())
        {
            _pages->detach();
        }
        else
        {
            addChild(_pages);
        }
        _tree->updateSizes();
        //updateSize();
    }
}
