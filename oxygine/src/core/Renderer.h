#pragma once
#include "oxygine-include.h"
#include <vector>

#include "math/Color.h"
#include "math/Rect.h"
#include "math/AffineTransform.h"
#include "NativeTexture.h"
#include "VideoDriver.h"

#if OXYGINE_NO_SUBPIXEL_RENDERING
#define SNAP_PIXEL(arg) floor(arg)
#else
#define SNAP_PIXEL(arg) (arg)
#endif

namespace oxygine
{
    typedef AffineTransform Transform;

#define MAKE_BLEND_MODE(source, dest) ((IVideoDriver::BT_##source << 16) | (IVideoDriver::BT_##dest))


    enum blend_mode
    {
        blend_disabled              = 0,
        blend_premultiplied_alpha   = MAKE_BLEND_MODE(ONE, ONE_MINUS_SRC_ALPHA),
        blend_alpha                 = MAKE_BLEND_MODE(SRC_ALPHA, ONE_MINUS_SRC_ALPHA),
        blend_add                   = MAKE_BLEND_MODE(ONE, ONE),
        blend_screen                = MAKE_BLEND_MODE(ONE, ONE_MINUS_SRC_COLOR),
        blend_multiply              = MAKE_BLEND_MODE(DST_COLOR, ONE_MINUS_SRC_ALPHA),
        blend_inverse               = MAKE_BLEND_MODE(ONE_MINUS_DST_COLOR, ZERO),
    };


    template<class V>
    void fillQuadT(V* pv, const RectF& srcRect, const RectF& destRect, const AffineTransform& transform, unsigned int rgba)
    {
        float u = srcRect.pos.x;
        float v = srcRect.pos.y;

        float du = srcRect.size.x;
        float dv = srcRect.size.y;

        V vt;
        vt.color = rgba;

        const Vector2& pos = destRect.pos;
        const Vector2& size = destRect.size;

        Vector2 p1(pos.x, pos.y);
        Vector2 p2(pos.x, pos.y + size.y);
        Vector2 p3(pos.x + size.x, pos.y);
        Vector2 p4(pos.x + size.x, pos.y + size.y);


        p1 = transform.transform(p1);
        p2 = transform.transform(p2);
        p3 = transform.transform(p3);
        p4 = transform.transform(p4);

        vt.z = 0;

        vt.x = SNAP_PIXEL(p1.x);
        vt.y = SNAP_PIXEL(p1.y);
        vt.u = u;
        vt.v = v;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p2.x);
        vt.y = SNAP_PIXEL(p2.y);
        vt.u = u;
        vt.v = v + dv;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p3.x);
        vt.y = SNAP_PIXEL(p3.y);
        vt.u = u + du;
        vt.v = v;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p4.x);
        vt.y = SNAP_PIXEL(p4.y);
        vt.u = u + du;
        vt.v = v + dv;
        *pv = vt;
        ++pv;
    }

    template<class V>
    void fillQuadZT(V* pv, const RectF& srcRect, const RectF& destRect, float Z, unsigned int rgba)
    {
        float u = srcRect.pos.x;
        float v = srcRect.pos.y;

        float du = srcRect.size.x;
        float dv = srcRect.size.y;

        V vt;
        vt.color = rgba;

        const Vector2& pos = destRect.pos;
        const Vector2& size = destRect.size;

        Vector2 p1(pos.x, pos.y);
        Vector2 p2(pos.x, pos.y + size.y);
        Vector2 p3(pos.x + size.x, pos.y);
        Vector2 p4(pos.x + size.x, pos.y + size.y);


        vt.z = Z;

        vt.x = SNAP_PIXEL(p1.x);
        vt.y = SNAP_PIXEL(p1.y);
        vt.u = u;
        vt.v = v;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p2.x);
        vt.y = SNAP_PIXEL(p2.y);
        vt.u = u;
        vt.v = v + dv;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p3.x);
        vt.y = SNAP_PIXEL(p3.y);
        vt.u = u + du;
        vt.v = v;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p4.x);
        vt.y = SNAP_PIXEL(p4.y);
        vt.u = u + du;
        vt.v = v + dv;
        *pv = vt;
        ++pv;
    }


    template<class V>
    void fillQuadT2(V* pv, const RectF& srcRect, const RectF& srcRect2, const RectF& destRect, const AffineTransform& transform, unsigned int rgba)
    {
        float u = srcRect.pos.x;
        float v = srcRect.pos.y;

        float du = srcRect.size.x;
        float dv = srcRect.size.y;

        float u2 = srcRect2.pos.x;
        float v2 = srcRect2.pos.y;

        float du2 = srcRect2.size.x;
        float dv2 = srcRect2.size.y;

        V vt;
        vt.color = rgba;

        const Vector2& pos = destRect.pos;
        const Vector2& size = destRect.size;

        Vector2 p1(pos.x, pos.y);
        Vector2 p2(pos.x, pos.y + size.y);
        Vector2 p3(pos.x + size.x, pos.y);
        Vector2 p4(pos.x + size.x, pos.y + size.y);


        p1 = transform.transform(p1);
        p2 = transform.transform(p2);
        p3 = transform.transform(p3);
        p4 = transform.transform(p4);

        vt.z = 0;

        vt.x = SNAP_PIXEL(p1.x);
        vt.y = SNAP_PIXEL(p1.y);
        vt.u = u;
        vt.v = v;
        vt.u2 = u2;
        vt.v2 = v2;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p2.x);
        vt.y = SNAP_PIXEL(p2.y);
        vt.u = u;
        vt.v = v + dv;
        vt.u2 = u2;
        vt.v2 = v2 + dv2;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p3.x);
        vt.y = SNAP_PIXEL(p3.y);
        vt.u = u + du;
        vt.v = v;
        vt.u2 = u2 + du2;
        vt.v2 = v2;
        *pv = vt;
        ++pv;

        vt.x = SNAP_PIXEL(p4.x);
        vt.y = SNAP_PIXEL(p4.y);
        vt.u = u + du;
        vt.v = v + dv;
        vt.u2 = u2 + du2;
        vt.v2 = v2 + dv2;
        *pv = vt;
        ++pv;
    }

    class IElementRenderer
    {
    public:
        virtual ~IElementRenderer() {}
        virtual void drawElement(const spNativeTexture& texture, unsigned int color, const RectF& src, const RectF& dest) = 0;
    };

    /**Returns View matrix where Left Top corner is (0,0), and right bottom is (w,h)*/
    Matrix makeViewMatrix(int w, int h, bool flipU = false);
}
