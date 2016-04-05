#include "Stage.h"
#include "core/oxygine.h"
#include "math/Rect.h"
#include "RenderState.h"
#include "Clock.h"
#include <sstream>
#include "STDMaterial.h"

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

        _material = STDMaterial::instance;

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

    void Stage::render(const Color* clearColor, const Rect& viewport, const Matrix& view, const Matrix& proj)
    {
        Material::setCurrent(0);

        IVideoDriver* driver = IVideoDriver::instance;
        driver->setViewport(viewport);

        if (clearColor)
            driver->clear(*clearColor);

        STDMaterial& mat = *STDMaterial::instance;
        mat.apply(0);
        mat.setViewProj(view, proj);

        timeMS t = getTimeMS();
        RenderState rs;
        rs.material = _material;
        Point ds = core::getDisplaySize();

        RectF clip(0.0f, 0.0f, (float)ds.x, (float)ds.y);
        rs.clip = &clip;

        if (_clipOuter)
        {
            driver->setScissorRect(&_viewport);
            clip = _viewport.cast<RectF>();
        }

        Actor::render(rs);

        mat.finish();

        _statRender = getTimeMS() - t;
    }

    void Stage::render(const Color& clearColor, const Rect& viewport)
    {
        //initialize projection and view matrix
        Matrix proj;
        Matrix::orthoLH(proj, (float)viewport.getWidth(), (float)viewport.getHeight(), 0.2f, 10000);
        Matrix view = makeViewMatrix(viewport.getWidth(), viewport.getHeight());
        render(&clearColor, viewport, view, proj);
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