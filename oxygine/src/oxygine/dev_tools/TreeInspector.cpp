#include "TreeInspector.h"
#include "../initActor.h"
#include "../actor/ColorRectSprite.h"
#include "../RenderState.h"
#include "../Clock.h"

#include "TreeInspectorPage.h"
#include "TreeInspectorPreview.h"

#include "../res/ResAnim.h"
#include "../res/ResFontBM.h"
#include "../res/Resources.h"

#include "../Image.h"
#include "../Font.h"
#include "../actor/Stage.h"

#include "../actor/TextField.h"
#include "../actor/DebugActor.h"

#include "../actor/SlidingActor.h"

namespace oxygine
{
    void printvec(const char* s, const Vector2& v)
    {
        //printf("%s: %.2f, %.2f\n", s, v.x, v.y);
    }


    RectF TreeInspector::calcBounds(Actor* actor)
    {
        OX_ASSERT(actor);

        RectF r(actor->getPosition(), actor->getSize());

        spActor c = actor->getFirstChild();
        while (c)
        {
            if (c->getVisible())
            {
                RectF cr = calcBounds(c.get());
                cr.pos += c->getPosition();
                r.unite(cr);
            }
            c = c->getNextSibling();
        }

        r.pos = r.pos - actor->getPosition();
        return r;
    }

    ResAnim* CreateResAnim(const char* file, int rows, int cols)
    {
        ResAnim* rs = new ResAnim(0);

        Image mt;
        file::buffer bf;
        file::read(file, bf);
        mt.init(bf);

        rs->init(&mt, cols, rows);

        return rs;
    }

    TreeInspector::TreeInspector(): _preview(0), _resSystem(0)
    {
        setName(getDefaultName());

        DebugActor::initialize();
        _resSystem = DebugActor::resSystem;

        setPriority(999);
        //setTouchEnabled(false);
        //setTouchChildrenEnabled(false);
    }

    void getDescendants(spActor actor, std::vector<spActor>& actors)
    {
        actors.push_back(actor);
        spActor child = actor->getFirstChild();
        while (child)
        {
            getDescendants(child, actors);
            child = child->getNextSibling();
        }
        //actor->removeChildren();
    }

    TreeInspector::~TreeInspector()
    {
    }

    void TreeInspector::render(const RenderState& rs_)
    {
        RenderState rs = rs_;
        IVideoDriver::instance->setScissorRect(0);

        Rect vp;
        IVideoDriver::instance->getViewport(vp);
        RectF clip = vp.cast<RectF>();
        rs.clip = &clip;

        ClipRectActor::render(rs);
    }

    void TreeInspector::init(const Vector2& size, spActor actor)
    {
        setSize(size);


        _rootPage = new TreeInspectorPage(this, 0);
        //_rootPage->setTouchChildrenEnabled(false);
        //_rootPage->setTouchEnabled(false);


        _rootPage->init(actor);

        _rootPage->setY(5);
        setWidth(_rootPage->getWidth());


        spSlidingActor slidingActor = new SlidingActor();
        slidingActor->setSize(size);
        slidingActor->setContent(_rootPage);

        addChild(slidingActor);

        _sliding = slidingActor;


        TextStyle style;
        style.font = _resSystem->getResFont("system");
        style.vAlign = TextStyle::VALIGN_TOP;

        updateSizes();
    }

    void TreeInspector::onAdded2Stage()
    {
        _getStage()->addEventListener(TouchEvent::WHEEL_DOWN, CLOSURE(this, &TreeInspector::wheel));
        _getStage()->addEventListener(TouchEvent::WHEEL_UP, CLOSURE(this, &TreeInspector::wheel));
    }

    void TreeInspector::onRemovedFromStage()
    {
        _getStage()->removeEventListeners(this);
    }

    void TreeInspector::updateSizes()
    {
        Vector2 pos = _rootPage->getPosition();
        Vector2 size = _rootPage->getSize();
        _rootPage->updateSizesNew();
        _rootPage->setPosition(pos);
        _rootPage->setSize(size + Vector2(0, 24));
        //_rootPage->setPosition(0, 20);
        _sliding->snap();
    }

    void TreeInspector::close(Event* ev)
    {
        detach();
        _getStage()->removeEventListeners(this);
        //return true;
    }

    void TreeInspector::wheel(Event* ev)
    {
        float y = _rootPage->getY();
        if (ev->type == TouchEvent::WHEEL_DOWN)
            y -= 10;
        else
            y += 10;

        _rootPage->setY(y);
    }

    Vector2 fitSize(const Vector2& destSize, const Vector2& src);
    void TreeInspector::showPreview(TreeInspectorPreview* item, bool show)
    {
        /*
        if (show == false && !_preview)
            return;

        if (_preview && _preview != item)
            return;

        if (show)
        {
            Vector2 pos = convertPosUp(item, _body.get(), item->getPosition(), false);

            item->_prevParent = item->getParent();
            item->detach();

            _body->addChild(item);

            item->setPosition(pos);

            Point ns = calcBoundSize(Point(300, 300), item->_item->getSize());
            float scale = ns.x / (float)item->getWidth();

            item->setScale(scale);

            _preview = item;
        }
        else
        {
            item->detach();

            item->_prevParent->addChild(item);
            item->setScale(1);

            item->setPosition(Point(0,0));

            _preview = 0;
        }
        */
    }
}
