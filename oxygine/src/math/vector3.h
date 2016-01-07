#pragma once
#include "oxygine_include.h"
#include "vector2.h"
namespace oxygine
{
    template <class T>
    class VectorT3
    {
        typedef VectorT3<T> vector3;
    public:
        typedef T type;


        VectorT3();
        VectorT3(const T*);
        VectorT3(T, T, T);

        VectorT3& operator+=(const VectorT3&);
        VectorT3& operator-=(const VectorT3&);

        VectorT3 operator + (const VectorT3&) const;
        VectorT3 operator - (const VectorT3&) const;
        VectorT3 operator - () const;

        VectorT3 operator * (T) const;
        VectorT3 operator / (T) const;

        VectorT3& operator*=(T);
        VectorT3& operator/=(T);

        template<class R>
        operator VectorT3<R> ()const;

        VectorT2<T> xy() const
        {
            return VectorT2<T>(x, y);
        }

        bool operator == (const VectorT3& r) const;
        bool operator != (const VectorT3& r) const;

        void normalize();
        void clamp(const VectorT3& min, const VectorT3& max);
        T length() const;
        T dot(const VectorT3& p)  const;

        inline T& operator[](int i) {return m[i];}
        inline const T& operator[](int i)const {return m[i];}

        //////////////////////////
        static VectorT3& cross(VectorT3& out, const VectorT3& v1, const VectorT3& v2);
        static T dot(const VectorT3& v1, const VectorT3& v2);
        static VectorT3& normalize(VectorT3& out, const VectorT3& v);
        static VectorT3& normalFromPoints(VectorT3& out, const VectorT3& v1, const VectorT3& v2, const VectorT3& v3);

        union
        {
            struct
            {
                T x, y, z;
            };
            T m[3];
        };
    };

    template<class T>
    template<class R>
    VectorT3<T>::operator VectorT3<R> ()const
    {
        return VectorT3<R>(R(x), R(y), R(z));
    }


    template<class T>
    bool VectorT3<T>::operator == (const VectorT3<T>& r) const
    {
        if (x == r.x && y == r.y && z == r.z)
            return true;
        return false;
    }

    template<class T>
    bool VectorT3<T>::operator != (const VectorT3<T>& r) const
    {
        return !(r == (*this));
    }

    template <class T>
	VectorT3<T>::VectorT3():
        x(0), y(0), z(0)
    {}

    template <class T>
    VectorT3<T>::VectorT3(const T* p):
        x(p[0]), y(p[1]), z(p[2])
    {
    }


    template <class T>
    VectorT3<T>::VectorT3(T X, T Y, T Z):
        x(X), y(Y), z(Z)
    {
    }

    template <class T>
    VectorT3<T>& VectorT3<T>::operator+=(const VectorT3& v)
    {
        x += v.x; y += v.y; z += v.z; return (*this);
    }

    template <class T>
    VectorT3<T>& VectorT3<T>::operator-=(const VectorT3& v)
    {
        x -= v.x; y -= v.y; z -= v.z; return (*this);
    }

    template <class T>
    VectorT3<T> VectorT3<T>::operator + (const VectorT3& v) const
    {
        return VectorT3(x + v.x, y + v.y, z + v.z);
    }

    template <class T>
    VectorT3<T> VectorT3<T>::operator - (const VectorT3& v) const
    {
        return VectorT3(x - v.x, y - v.y, z - v.z);
    }

    template <class T>
    VectorT3<T> VectorT3<T>::operator - () const
    {
        return VectorT3(-x, -y, -z);
    }

    template <class T>
    VectorT3<T> VectorT3<T>::operator * (T s) const
    {
        return VectorT3(x * s, y * s, z * s);
    }

    template <class T>
    VectorT3<T> VectorT3<T>::operator / (T v) const
    {
        T s = T(1) / v;
        return VectorT3(x * s, y * s, z * s);
    }


    template <class T>
    VectorT3<T>& VectorT3<T>::operator*=(T s)
    {
        x *= s; y *= s; z *= s; return (*this);
    }

    template <class T>
    VectorT3<T>& VectorT3<T>::operator/=(T s)
    {
        T is = T(1.0) / s;
        x *= is; y *= is; z *= is; return (*this);
    }

    template <class T>
    void VectorT3<T>::normalize()
    {
        normalize(*this, *this);
    }

    template <class T>
    T VectorT3<T>::length() const
    {
        return scalar::sqrt(x * x + y * y + z * z);
    }

    template <class T>
    T VectorT3<T>::dot(const VectorT3& p) const
    {
        return VectorT3<T>::dot(*this, p);
    }


    template <class T>
    inline VectorT3<T>& VectorT3<T>::cross(VectorT3& out, const VectorT3& v1, const VectorT3& v2)
    {
        out.x = v1.y * v2.z - v1.z * v2.y;
        out.y = v1.z * v2.x - v1.x * v2.z;
        out.z = v1.x * v2.y - v1.y * v2.x;
        return out;
    }

    template <class T>
    inline void VectorT3<T>::clamp(const VectorT3& min, const VectorT3& max)
    {
        if (x > max.x)
            x = max.x;
        if (y > max.y)
            y = max.y;
        if (z > max.z)
            z = max.z;

        if (x < min.x)
            x = min.x;
        if (y < min.y)
            y = min.y;
        if (z < min.z)
            z = min.z;
    }

    template <class T>
    inline T VectorT3<T>::dot(const VectorT3& v1, const VectorT3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    template <class T>
    inline VectorT3<T>& VectorT3<T>::normalize(VectorT3<T>& out, const VectorT3<T>& v)
    {
        T norm = T(1.0) / scalar::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        out = v;
        out.x *= norm;
        out.y *= norm;
        out.z *= norm;
        return out;
    }

    template <class T>
    inline VectorT3<T>& VectorT3<T>::normalFromPoints(VectorT3& out, const VectorT3& v1, const VectorT3& v2, const VectorT3& v3)
    {
        vector3 b1 = v1 - v2;
        vector3 b2 = v3 - v2;
        vector3 cross;
        vector3::cross(&cross, b2, b1);
        vector3::normalize(&cross, cross);

        out.x = cross.x;
        out.y = cross.y;
        out.z = cross.z;

        return out;
    }

    typedef VectorT3<float> Vector3;
    typedef VectorT3<double> VectorD3;
}
