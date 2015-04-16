#include "Stage.h"
#include "core/oxygine.h"
#include "core/Renderer.h"
#include "math/Rect.h"
#include "RenderState.h"
#include "STDRenderer.h"
#include "Clock.h"
#include <sstream>

#ifdef OXYGINE_SDL
#include "SDL.h"
#endif

namespace oxygine
{
    spStage Stage::instance;

    Stage::Stage(bool autoReset) : _statUpdate(0), _statRender(0), _clipOuter(false), _viewport(0, 0, 0, 0) //, _active(true)
    {
        spClock clock = new Clock();
        setClock(clock);
        setName("Stage");

        //each mobile application should handle focus lost
        //and free/restore GPU resources
        if (autoReset)
        {
            addEventListener(Stage::DEACTIVATE, CLOSURE(this, &Stage::onDeactivate));
            addEventListener(Stage::ACTIVATE, CLOSURE(this, &Stage::onActivate));
        }
        _stage = this;

#ifdef OXYGINE_SDL
        _window = 0;
#endif
    }

#if OXYGINE_SDL
    void Stage::associateWithWindow(SDL_Window* wnd)
    {
        _window = wnd;
        SDL_SetWindowData(wnd, "_", this);
        addRef();
    }

    SDL_Window* Stage::getAssociatedWindow() const
    {
        if (_window)
            return _window;
        return core::getWindow();
    }
#endif

    Stage::~Stage()
    {

    }

    void Stage::onDeactivate(Event*)
    {
        //_active = false;
        core::reset();
    }

    void Stage::onActivate(Event*)
    {
        core::restore();
        //_active = true;
    }

    std::string Stage::dump(const dumpOptions& opt) const
    {
        std::stringstream st;

        st << "{Stage}\n";
        //st << " displaySize=(" << _realDisplaySize.x << "," << _realDisplaySize.y << ")";
        st << Actor::dump(opt);

        return st.str();
    }

    Rect    Stage::calcCenteredViewport(const Point& displaySize, const Point& gameSize)
    {
        float width = (float)displaySize.x;
        float height = (float)displaySize.y;

        float scaleFactorX = width / gameSize.x;
        float scaleFactorY = height / gameSize.y;

        float scaleFactor = scaleFactorX < scaleFactorY ? scaleFactorX : scaleFactorY;
        Vector2 size = gameSize * scaleFactor;

        Vector2 free = displaySize.cast<Vector2>() - size;

        return Rect((free / 2).cast<Point>(), size.cast<Point>());
    }

    void Stage::init(const Point& displaySize, const Point& gameSize)
    {
        //_realDisplaySize = displaySize;
        setSize(gameSize);

        _viewport = calcCenteredViewport(displaySize, gameSize);

        float scaleFactor = _viewport.size.x / (float)gameSize.x;

        setScale(scaleFactor);
        setPosition(_viewport.pos);
    }

    bool Stage::isOn(const Vector2& localPosition)
    {
        return true;
    }


    RectF Stage::getDestRect() const
    {
        Vector2 s = getSize() + getPosition();
        RectF r = calcDestRectF(RectF(-getPosition(), s), s);
        return r;
    }

    /*
    bool Stage::handleEvent(const EventState &es)
    {
        bool handled = Actor::handleEvent(es);
        return handled;
    }
    */

    void Stage::render(Renderer& r)
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

    void Stage::render(const Color& clearColor, const Rect& viewport)
    {
        //if (!_active)
        //  return;

        IVideoDriver* driver = IVideoDriver::instance;

        driver->setViewport(viewport);
        driver->clear(clearColor);

        STDRenderer renderer(driver);
        renderer.initCoordinateSystem(viewport.getWidth(), viewport.getHeight());
        renderer.begin(0);
        render(renderer);
        renderer.end();
    }

    void Stage::cleanup()
    {
    }

    void Stage::update()
    {
        timeMS t = getTimeMS();
        UpdateState us;
        Actor::update(us);

        _statUpdate = getTimeMS() - t;
    }
}