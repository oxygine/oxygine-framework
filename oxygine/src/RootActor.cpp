#include "RootActor.h"
#include "core/Renderer.h"
#include "math/Rect.h"
#include "RenderState.h"
#include <sstream>

namespace oxygine
{
	spRootActor RootActor::instance;

	RootActor::RootActor():_statUpdate(0), _statRender(0), _clipOuter(false), _viewport(0,0,0,0)
	{
		spClock clock = new Clock();
		setClock(clock);
		setName("RootActor");

		//addEventHandler(new EventHandler());
	}

	RootActor::~RootActor()
	{

	}

	std::string RootActor::dump(const dumpOptions &opt) const
	{
		stringstream st;

		st << "{RootActor}\n";
		//st << " displaySize=(" << _realDisplaySize.x << "," << _realDisplaySize.y << ")";
		st << Actor::dump(opt);

		return st.str();
	}

	Rect	RootActor::calcCenteredViewport(const Point &displaySize, const Point &gameSize)
	{
		float width = (float)displaySize.x;
		float height = (float)displaySize.y;

		float scaleFactorX = width / gameSize.x;
		float scaleFactorY = height / gameSize.y;

		float scaleFactor = scaleFactorX < scaleFactorY ? scaleFactorX : scaleFactorY;
		Vector2 size = gameSize * scaleFactor;

		Vector2 free = displaySize.cast<Vector2>() - size;

		return Rect((free/2).cast<Point>(), size.cast<Point>());
	}

	void RootActor::init(const Point &displaySize, const Point &gameSize)
	{
		//_realDisplaySize = displaySize;
		setSize(gameSize);

		_viewport = calcCenteredViewport(displaySize, gameSize);

		float scaleFactor = _viewport.size.x / (float)gameSize.x;

		setScale(scaleFactor);
		setPosition(_viewport.pos);
	}

	bool RootActor::isOn(const Vector2 &localPosition)
	{
		return true;
	}
		

	RectF RootActor::getDestRect() const
	{
		Vector2 s = getSize() + getPosition();
		RectF r = calcDestRectF(RectF(-getPosition(), s), s);
		return r;
	}

	/*
	bool RootActor::handleEvent(const EventState &es)
	{
		bool handled = Actor::handleEvent(es);
		return handled;
	}
	*/

	void RootActor::render(Renderer &r)
	{
		timeMS t = getTimeMS();
		RenderState rs;
		rs.renderer = &r;
		Point ds = core::getDisplaySize();

		RectF clip(0.0f, 0.0f, (float)ds.x, (float)ds.y);
		rs.clip = &clip;

		if (_clipOuter)
		{
			r.getDriver()->setScissorRect(&_viewport);
			clip = _viewport.cast<RectF>();
		}
		
		Actor::render(rs);

		_statRender = getTimeMS() - t;
	}

	void RootActor::cleanup()
	{
	}

	void RootActor::update()
	{
		timeMS t = getTimeMS();
		UpdateState us;		
		Actor::update(us);

		_statUpdate = getTimeMS() - t;		
	}
}