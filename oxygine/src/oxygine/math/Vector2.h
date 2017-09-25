#pragma once
#include "../oxygine-include.h"
#include "ScalarMath.h"
//#include "../math/vector3.h"

namespace oxygine
{
    template <class T>
    class VectorT2
    {
        typedef VectorT2<T> vector2;
    public:
        typedef T type;
        //////////////
        VectorT2();
        VectorT2(T, T);

        VectorT2& operator+=(const VectorT2&);
        VectorT2& operator-=(const VectorT2&);

        VectorT2 operator + (const VectorT2&) const;
        VectorT2 operator - (const VectorT2&) const;
        VectorT2 operator - () const;

        void set(T x_, T y_) {x = x_; y = y_;}
        void setZero() {x = 0; y = 0;}

        template <class R>
        VectorT2 operator * (R s) const {VectorT2 r(*this); r.x = type(r.x * s); r.y = type(r.y * s); return r;}
        template <class R>
        VectorT2 operator / (R s) const {VectorT2 r(*this); r.x /= s; r.y /= s; return r;}

        template <class R>
        VectorT2 operator *= (R s) {x *= s; y *= s; return (*this);}
        template <class R>
        VectorT2 operator /= (R s) {x /= s; y /= s; return (*this);}

        VectorT2 mult(const VectorT2& r) const {return VectorT2(x * r.x, y * r.y);}
        VectorT2 div(const VectorT2& r) const {return VectorT2(x / r.x, y / r.y);}

        operator VectorT2<float> () const {return this->cast< VectorT2<float> >();}

        template<typename R>
        R cast() const
        {
            typedef R vec2;
            typedef typename R::type vec2type;
            return vec2(vec2type(x), vec2type(y));
        }


        bool operator == (const VectorT2& r) const;
        bool operator != (const VectorT2& r) const;

        //inline T &operator[](int i){return m[i];}
        //inline const T &operator[](int i)const{return m[i];}

        T length() const {return (T)scalar::sqrt(x * x + y * y);}
        T sqlength() const {return dot(*this);}

        void normalize() { normalize(*this, *this); }
        void normalizeTo(T len) { normalize(); *this *= len; }
        VectorT2 normalized() const {VectorT2 t = *this; t.normalize(); return t;}

        float distance(const VectorT2& v) const { return VectorT2(x - v.x, y - v.y).length();}
        T dot(const VectorT2& vr) const {return dot(*this, vr);}

        static T dot(const VectorT2& v1, const VectorT2& v2);
        static VectorT2& normalize(VectorT2& out, const VectorT2& v);

        struct
        {
            T x, y;
        };
    };

    /*
    template<class T>
    VectorT2<T>::operator VectorT2<float> ()const
    {
        return VectorT2<float>(float(x), float(y));
    }
    */

    template<class T>
    bool VectorT2<T>::operator == (const VectorT2<T>& r) const
    {
        if (x == r.x && y == r.y)
            return true;
        return false;
    }

    template<class T>
    bool VectorT2<T>::operator != (const VectorT2<T>& r) const
    {
        if (x != r.x || y != r.y)
            return true;
        return false;
    }

    template <class T>
    VectorT2<T>::VectorT2(): x(0), y(0)
    {}

    template <class T>
    VectorT2<T>::VectorT2(T X, T Y):
        x(X), y(Y)
    {
    }

    template <class T>
    VectorT2<T>& VectorT2<T>::operator+=(const VectorT2& v)
    {
        x += v.x; y += v.y; return (*this);
    }

    template <class T>
    VectorT2<T>& VectorT2<T>::operator-=(const VectorT2& v)
    {
        x -= v.x; y -= v.y; return (*this);
    }

    template <class T>
    VectorT2<T> VectorT2<T>::operator + (const VectorT2& v) const
    {
        return VectorT2(x + v.x, y + v.y);
    }

    template <class T>
    VectorT2<T> VectorT2<T>::operator - (const VectorT2& v) const
    {
        return VectorT2(x - v.x, y - v.y);
    }

    template <class T>
    VectorT2<T> VectorT2<T>::operator - () const
    {
        return VectorT2<T>(-x, -y);
    }

    template <class T>
    inline T VectorT2<T>::dot(const VectorT2& v1, const VectorT2& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    template <class T>
    inline VectorT2<T>& VectorT2<T>::normalize(VectorT2<T>& out, const VectorT2<T>& v)
    {
        T norm = T(1.0) / scalar::sqrt(v.x * v.x + v.y * v.y);
        out = v;
        /*
        if (norm < 0.0001)
        {
            return out;
        }
        */

        out.x *= norm;
        out.y *= norm;
        return out;
    }


    typedef VectorT2<float> Vector2;
    typedef VectorT2<double> VectorD2;
    typedef VectorT2<int> Point;
    typedef VectorT2<short> PointS;
}
