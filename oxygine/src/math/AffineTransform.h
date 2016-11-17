#pragma once
#include "oxygine-include.h"
#include "Vector2.h"
#include "Matrix.h"

namespace oxygine
{
    template<class T>
    class AffineTransformT
    {
    public:

        typedef VectorT2<T> vector2;
        typedef AffineTransformT<T> affineTransform;
        typedef MatrixT<T> matrix;

        AffineTransformT() {}
        AffineTransformT(T a_, T b_, T c_, T d_, T x_, T y_): a(a_), b(b_), c(c_), d(d_), x(x_), y(y_) {}


        explicit AffineTransformT(const matrix& m)
        {
            a = m.ml[0];
            b = m.ml[1];
            c = m.ml[4];
            d = m.ml[5];
            x = m.ml[12];
            y = m.ml[13];
        }

        void identity()
        {
            a = T(1);
            b = T(0);
            c = T(0);
            d = T(1);
            x = T(0);
            y = T(0);
        }

        static affineTransform getIdentity()
        {
            affineTransform t;
            t.identity();
            return t;
        }

        void translate(const vector2& v)
        {
            x += a * v.x + c * v.y;
            y += b * v.x + d * v.y;
        }

        affineTransform translated(const vector2& v) const
        {
            affineTransform t = *this;
            t.translate(v);
            return t;
        }

        void scale(const vector2& v)
        {
            a *= v.x;
            b *= v.x;
            c *= v.y;
            d *= v.y;
        }

        affineTransform scaled(const vector2& v) const
        {
            affineTransform t = *this;
            t.scale(v);
            return t;
        }

        void rotate(T v)
        {
            T sin_ = scalar::sin(v);
            T cos_ = scalar::cos(v);

            affineTransform rot(cos_, sin_, -sin_, cos_, 0, 0);
            *this = *this * rot;
        }

        affineTransform rotated(T v) const
        {
            affineTransform t = *this;
            t.rotate(v);
            return t;
        }

        void invert()
        {
            affineTransform t = *this;

            T det = T(1) / (t.a * t.d - t.b * t.c);

            a = det * t.d;
            b = -det * t.b;
            c = -det * t.c;
            d = det * t.a;
            x = det * (t.c * t.y - t.d * t.x);
            y = det * (t.b * t.x - t.a * t.y);
        }

        affineTransform inverted() const
        {
            affineTransform t = *this;
            t.invert();
            return t;
        }

        operator matrix() const
        {
            return toMatrix();
        }

        matrix toMatrix() const
        {
            return matrix(
                       a, b, 0, 0,
                       c, d, 0, 0,
                       0, 0, 1, 0,
                       x, y, 0, 1
                   );
        }


        static affineTransform& multiply(affineTransform& out, const affineTransform& t1, const affineTransform& t2)
        {
            out.a = t1.a * t2.a + t1.b * t2.c;
            out.b = t1.a * t2.b + t1.b * t2.d;
            out.c = t1.c * t2.a + t1.d * t2.c;
            out.d = t1.c * t2.b + t1.d * t2.d;
            out.x = t1.x * t2.a + t1.y * t2.c + t2.x;
            out.y = t1.x * t2.b + t1.y * t2.d + t2.y;

            return out;
        }


        affineTransform operator * (const affineTransform& t2) const
        {
            affineTransform r;
            multiply(r, *this, t2);
            return r;
        }

        vector2 transform(const vector2& v) const
        {
            return vector2(
                       a * v.x + c * v.y + x,
                       b * v.x + d * v.y + y);
        }


        T a, b, c, d;
        T x, y;
    };

    typedef AffineTransformT<float> AffineTransform;
}
