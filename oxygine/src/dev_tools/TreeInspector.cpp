#include "TreeInspector.h"
#include "initActor.h"
#include "ColorRectSprite.h"
#include "RenderState.h"
#include "Clock.h"

#include "TreeInspectorPage.h"
#include "TreeInspectorPreview.h"

#include "res/ResAnim.h"
#include "res/ResFontBM.h"
#include "res/Resources.h"

#include "MemoryTexture.h"
#include "Font.h"
#include "RootActor.h"

#include "TextActor.h"
#include "DebugActor.h"

#include "SlidingActor.h"

namespace oxygine
{
	void printvec(const char *s, const Vector2 &v)
	{
		//printf("%s: %.2f, %.2f\n", s, v.x, v.y);
	}


	RectF getBounds(Actor *a)
	{
		return RectF(a->getPosition(), a->getSize());
	}

	RectF TreeInspector::calcBounds(Actor *actor)
	{
		OX_ASSERT(actor);

		RectF r = getBounds(actor);

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

	ResAnim *CreateResAnim(const char *file, int rows, int cols)
	{
		ResAnim *rs = new ResAnim(0);

		MemoryTexture mt;
		file::buffer bf;
		file::read(file, bf);
		mt.init(bf);

		rs->init(&mt, cols, rows);

		return rs;
	}

	TreeInspector::TreeInspector():_preview(0), _resSystem(0)
	{	
		setName(getDefaultName());
		
		DebugActor::initialize();
		_resSystem = DebugActor::resSystem;

		setPriority(999);
		//setInputEnabled(false);
		//setInputChildrenEnabled(false);
	}

	void getDescendants(spActor actor, vector<spActor> &actors)
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

	void TreeInspector::render(const RenderState &rs_)
	{
		RenderState rs = rs_;
		rs.renderer->getDriver()->setScissorRect(0);
		
		Rect vp;
		rs.renderer->getDriver()->getViewport(vp);		
		rs.clip = vp.cast<RectF>();		

		ClipRectActor::render(rs);
	}

	void TreeInspector::init(const Vector2 &size, spActor actor)
	{
		setSize(size);

		
		_rootPage = new TreeInspectorPage(this, 0);		
		//_rootPage->setInputChildrenEnabled(false);
		//_rootPage->setInputEnabled(false);

		
		_rootPage->init(actor);
		
		_rootPage->setY(5);
		setWidth(_rootPage->getWidth());

		
		spSlidingActor slidingActor = new SlidingActor();
		slidingActor->setSize(size);
		slidingActor->setContent(_rootPage);
		
		addChild(slidingActor);
		
		_sliding = slidingActor;
		

		TextStyle style;
		style.font = _resSystem->getResFont("system")->getFont();
		style.vAlign = TextStyle::VALIGN_TOP;

		

		addEventListener(TouchEvent::WHEEL_DOWN, CLOSURE(this, &TreeInspector::wheel));
		addEventListener(TouchEvent::WHEEL_UP, CLOSURE(this, &TreeInspector::wheel));

		updateSizes();
	}

	void TreeInspector::updateSizes()
	{
		Vector2 pos = _rootPage->getPosition();
		Vector2 size = _rootPage->getSize();
		_rootPage->updateSizesNew();
		_rootPage->setPosition(pos);
		_rootPage->setSize(size);
		//_rootPage->setPosition(0, 20);
		_sliding->snap();
	}
	
	void TreeInspector::close(Event *ev)
	{
		detach();
		//return true;
	}

	void TreeInspector::wheel(Event *ev)
	{
		float y = _rootPage->getY();
		if (ev->type == TouchEvent::WHEEL_DOWN)
			y -= 10;
		else
			y += 10;

		_rootPage->setY(y);
	}

	Vector2 fitSize(const Vector2 &destSize, const Vector2 &src);
	void TreeInspector::showPreview(TreeInspectorPreview *item, bool show)
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
