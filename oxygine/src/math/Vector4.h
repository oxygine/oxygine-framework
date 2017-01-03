#pragma once
#include "oxygine-include.h"
#include "Vector3.h"
namespace oxygine
{
    template <class T>
    class VectorT4
    {
        typedef VectorT4<T> vector4;
        typedef VectorT3<T> vector3;

    public:

        VectorT4();
        VectorT4(const T*);
        VectorT4(const VectorT3<T>&, const T&);
        VectorT4(const T&, const T&, const T&, const T&);

        VectorT4& operator+=(const VectorT4&);
        VectorT4& operator-=(const VectorT4&);

        VectorT4 operator + (const VectorT4&) const;
        VectorT4 operator - (const VectorT4&) const;

        VectorT4& operator*=(T);
        VectorT4& operator/=(T);

        VectorT4 operator * (T) const;
        VectorT4 operator / (T) const;

        VectorT3<T> xyz()const;

        inline T& operator[](int i) {return m[i];}
        inline const T& operator[](int i)const {return m[i];}

        operator T* ()const;
        operator const T* () const;

        static T dot(const VectorT4& v1, const VectorT4& v2);
        static VectorT4* planeFromPoints(VectorT4* pOut, const vector3& v1, const vector3& v2, const vector3& v3);

        template<typename D> friend D& operator << (D& os, VectorT4<T>* p);
        template<typename D> friend D& operator << (D& os, VectorT4<T>& p);

        union
        {
            struct
            {
                T x, y, z, w;
            };
            T m[4];
        };

    };

    template <class T>
    VectorT4<T>::operator T* () const
    {
        return m;
    }

    template <class T>
    VectorT4<T>::operator const T* () const
    {
        return m;
    }

    template <class T>
    VectorT3<T> VectorT4<T>::xyz() const
    {
        return VectorT3<T>(x, y, z);
    }

    template <class T>
    VectorT4<T>::VectorT4()
    {}


    template <class T>
    VectorT4<T>::VectorT4(const T& X, const T& Y, const T& Z, const T& W):
        x(X), y(Y), z(Z), w(W)
    {
    }

    template <class T>
    VectorT4<T>::VectorT4(const T* p): x(p[0]), y(p[1]), z(p[2]), w(p[3])
    {
    }

    template <class T>
    VectorT4<T>::VectorT4(const VectorT3<T>& v, const T& W):
        x(v.x), y(v.y), z(v.z), w(W)
    {
    }


    template <class T>
    VectorT4<T>& VectorT4<T>::operator+=(const VectorT4& v)
    {
        x += v.x; y += v.y; z += v.z; w += v.w; return (*this);
    }

    template <class T>
    VectorT4<T>& VectorT4<T>::operator-=(const VectorT4& v)
    {
        x -= v.x; y -= v.y; z -= v.z; w -= v.w; return (*this);
    }

    template <class T>
    VectorT4<T> VectorT4<T>::operator + (const VectorT4& v) const
    {
        return VectorT4(x + v.x, y + v.y, z + v.z, w + v.w);
    }

    template <class T>
    VectorT4<T> VectorT4<T>::operator - (const VectorT4& v) const
    {
        return VectorT4(x - v.x, y - v.y, z - v.z, w - v.w);
    }

    template <class T>
    VectorT4<T>& VectorT4<T>::operator*=(T s)
    {
        x *= s; y *= s; z *= s; w *= s; return (*this);
    }

    template <class T>
    VectorT4<T>& VectorT4<T>::operator/=(T s)
    {
        T is = T(1.0) / s;
        x *= is; y *= is; z *= is; w *= is; return (*this);
    }

    template <class T>
    VectorT4<T> VectorT4<T>::operator * (T s) const
    {
        return VectorT4(x * s, y * s, z * s, w * s);
    }

    template <class T>
    VectorT4<T> VectorT4<T>::operator / (T v) const
    {
        T s = T(1) / v;
        return VectorT4(x * s, y * s, z * s, w * s);
    }

    template <class T>
    inline T VectorT4<T>::dot(const VectorT4& v1, const VectorT4& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    template <class T>
    inline VectorT4<T>* VectorT4<T>::planeFromPoints(VectorT4* pOut, const vector3& v1, const vector3& v2, const vector3& v3)
    {
        vector3 normal;
        vector3::normalFromPoints(&normal, v1, v2, v3);
        T dist = normal.dot(v1);

        *pOut = vector4(normal, dist);

        return pOut;
    }

    typedef VectorT4<float> Vector4;
    typedef VectorT4<double> VectorD4;
}
