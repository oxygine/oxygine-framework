#pragma once
#include "oxygine-include.h"
#include "STDRenderer.h"

namespace oxygine
{
    class ClipUV
    {
    public:
        ClipUV() {}
        ClipUV(const Vector2& a, const Vector2& b, const Vector2& c,
               const Vector2& a_uv, const Vector2& b_uv, const Vector2& c_uv) : _uvA(a_uv)
        {
            _dAB = b - a;
            _dAC = c - a;

            float v = 1.0f / (_dAB.x * _dAC.y - _dAB.y * _dAC.x);
            _dAB *= v;
            _dAC *= v;

            _dac.x = _dAC.x * a.y - _dAC.y * a.x;
            _dac.y = -(_dAB.x * a.y - _dAB.y * a.x);

            _dAB *= -1;

            _dAB.x *= -1;
            _dAC.x *= -1;

            _duvAB = b_uv - _uvA;
            _duvAC = c_uv - _uvA;
        }

        void get(Vector3* vec) const
        {
            vec[0] = Vector3(_dac.x, _dAC.y, _dAC.x);
            vec[1] = Vector3(_dac.y, _dAB.y, _dAB.x);

            vec[2] = Vector3(_uvA.x, _duvAB.x, _duvAC.x);
            vec[3] = Vector3(_uvA.y, _duvAB.y, _duvAC.y);
        }

        Vector2 calc(const Vector2& pos) const
        {
            //float a = _dac.x + _dAC.y * pos.x - _dAC.x * pos.y;
            //float b = _dac.y + _dAB.y * pos.x - _dAB.x * pos.y;

            //Vector3 q =

            float a = Vector3(_dac.x, _dAC.y, _dAC.x).dot(Vector3(1, pos.x, pos.y));
            float b = Vector3(_dac.y, _dAB.y, _dAB.x).dot(Vector3(1, pos.x, pos.y));

            //float u = _uvA.x + _duvAB.x * a + _duvAC.x * b;
            //float v = _uvA.y + _duvAB.y * a + _duvAC.y * b;
            float u = Vector3(_uvA.x, _duvAB.x, _duvAC.x).dot(Vector3(1, a, b));
            float v = Vector3(_uvA.y, _duvAB.y, _duvAC.y).dot(Vector3(1, a, b));

            //Matrix

            return Vector2(u, v);
        }

    protected:
        Vector2 _dac;
        Vector2 _uvA;

        Vector2 _dAB;
        Vector2 _dAC;
        Vector2 _duvAB;
        Vector2 _duvAC;
    };

    class MaskedRenderer : public STDRenderer
    {
    public:
        MaskedRenderer(spNativeTexture mask, const RectF& srcRect, const RectF& destRect, const Transform& t, bool channelR, IVideoDriver*);

        //void draw(const RState *rs, const RectF &destRect) OVERRIDE;

    protected:
        void preDrawBatch();
        //void addVertices(const void *data, unsigned int size, const VertexDeclaration *decl) OVERRIDE;

        spNativeTexture _mask;
        RectF _clipMask;
        ClipUV _clipUV;
    };
}
