#pragma once
#include "../oxygine-include.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <algorithm>
#include <math.h>
#include <stdlib.h>

namespace oxygine
{
    template <class T>
    class MatrixT
    {
        typedef VectorT2<T> vector2;
        typedef VectorT3<T> vector3;
        typedef VectorT4<T> vector4;
        typedef MatrixT<T> matrix;

    public:

        MatrixT();
        MatrixT(const T*);
        MatrixT(
            T _11, T _12, T _13, T _14,
            T _21, T _22, T _23, T _24,
            T _31, T _32, T _33, T _34,
            T _41, T _42, T _43, T _44);

        operator T* ()const;
        operator const T* () const;

        MatrixT operator * (const MatrixT& m) const;
        vector4& operator[](int row);

        void identity();
        void inverse();
        void transpose();
        void translate(const vector3&);
        void scale(const vector3&);
        void buildSRT(const vector3& scale, T angle, const vector3& t);

        MatrixT inversed() const;
        MatrixT transposed() const;

        vector2 transform(const vector2&)const;
        vector3 transformVec3(const vector3&)const;
        vector4 transformVec4(const vector4&)const;
        vector3 getTranslation()const;

        static MatrixT& lookAtLH(
            MatrixT& out,
            const vector3& Eye,
            const vector3& At,
            const vector3& Up);

        static MatrixT& lookAtRH(
            MatrixT& out,
            const vector3& Eye,
            const vector3& At,
            const vector3& Up);

        static MatrixT ident();
        static MatrixT& inverse(MatrixT& out, const MatrixT& in);
        static MatrixT& transpose(MatrixT& out, const MatrixT& in);
        static MatrixT& rotationX(MatrixT& out, T angle);
        static MatrixT& rotationY(MatrixT& out, T angle);
        static MatrixT& rotationZ(MatrixT& out, T angle);
        static MatrixT& translation(MatrixT& out, const vector3& v);
        static MatrixT& scaling(MatrixT& out, const vector3& v);

        static MatrixT& perspectiveFovLH(MatrixT& out, T fovy, T aspect, T znear, T zfar);
        static MatrixT& perspectiveFovRH(MatrixT& out, T fovy, T aspect, T znear, T zfar);

        static MatrixT& perspectiveOffCenterLH(MatrixT& out, T left, T right, T bottom, T top, T znear, T zfar);
        static MatrixT& perspectiveOffCenterRH(MatrixT& out, T left, T right, T bottom, T top, T znear, T zfar);

        static MatrixT& orthoLH(MatrixT& out, T width, T height, T zNear, T zFar);

        static vector2& transformVec2(vector2& out, const vector2& in, const MatrixT& mat);
        static vector3& transformVec3(vector3& out, const vector3& in, const MatrixT& mat);
        static vector4& transformVec4(vector4& out, const vector4& in, const MatrixT& mat);


        union
        {
            struct
            {
                T m[4][4];
            };
            struct
            {
                T ml[16];
            };
            struct
            {
                T
                m11, m12, m13, m14,
                     m21, m22, m23, m24,
                     m31, m32, m33, m34,
                     m41, m42, m43, m44;
            };
        };

    };

    template <class T>
    MatrixT<T>::MatrixT()
    {
    }

    template <class T>
    MatrixT<T>::MatrixT(const T* p)
    {
        for (int i = 0; i < 16; ++i)
            ml[i] = p[i];
    }

    template <class T>
    MatrixT<T>::MatrixT(T _11, T _12, T _13, T _14,
                        T _21, T _22, T _23, T _24,
                        T _31, T _32, T _33, T _34,
                        T _41, T _42, T _43, T _44):
        m11(_11), m12(_12), m13(_13), m14(_14),
        m21(_21), m22(_22), m23(_23), m24(_24),
        m31(_31), m32(_32), m33(_33), m34(_34),
        m41(_41), m42(_42), m43(_43), m44(_44)
    {
    }


    template <class T>
    void MatrixT<T>::identity()
    {
        vector4* rows = (vector4*)ml;
        rows[0] = VectorT4<T>(1, 0, 0, 0);
        rows[1] = VectorT4<T>(0, 1, 0, 0);
        rows[2] = VectorT4<T>(0, 0, 1, 0);
        rows[3] = VectorT4<T>(0, 0, 0, 1);
    }

    template <class T>
    void MatrixT<T>::inverse()
    {
        inverse(*this, *this);
    }




    template <class T>
    MatrixT<T> MatrixT<T>::inversed() const
    {
        matrix m;
        inverse(m, *this);
        return m;
    }

    template <class T>
    MatrixT<T> MatrixT<T>::ident()
    {
        MatrixT m;
        m.identity();
        return m;
    }


    template <class T>
    void MatrixT<T>::transpose()
    {
        transpose(*this, *this);
    }


    template <class T>
    MatrixT<T> MatrixT<T>::transposed() const
    {
        matrix m;
        transpose(m, *this);
        return m;
    }

    template <class T>
    void MatrixT<T>::translate(const vector3& t)
    {
        matrix tm;
        matrix::translation(tm, t);
        *this = *this * tm;
    }

    template <class T>
    void MatrixT<T>::scale(const vector3& s)
    {
        matrix sm;
        matrix::scaling(sm, s);
        *this = *this * sm;
    }

    template <class T>
    void MatrixT<T>::buildSRT(const vector3& s, T angle, const vector3& t)
    {
        matrix sm, rm, tm;
        matrix::scaling(&sm, s);
        matrix::rotationY(&rm, angle);
        matrix::translation(&tm, t);

        *this = sm * rm * tm;
    }

    template <class T>
    VectorT2<T> MatrixT<T>::transform(const vector2& v)const
    {
        vector2 out;
        transformVec2(out, v, *this);
        return out;
    }

    template <class T>
    VectorT3<T> MatrixT<T>::transformVec3(const vector3& v)const
    {
        vector3 out;
        transformVec3(out, v, *this);
        return out;
    }

    template <class T>
    VectorT4<T> MatrixT<T>::transformVec4(const vector4& v)const
    {
        vector4 out;
        transformVec4(out, v, *this);
        return out;
    }

    template <class T>
    VectorT3<T> MatrixT<T>::getTranslation()const
    {
        vector4* rows = (vector4*)ml;
        return vector3(rows[3].x, rows[3].y, rows[3].z);
    }

#define rcDot4(r, c) (mrows[r].x * nrows[0][c] + mrows[r].y * nrows[1][c] + mrows[r].z * nrows[2][c] + mrows[r].w * nrows[3][c])

    template <class T>
    MatrixT<T> MatrixT<T>::operator *(const MatrixT<T>& n) const
    {
        const vector4* mrows = (vector4*)ml;
        const vector4* nrows = (vector4*)n.ml;
        matrix mat
        (
            rcDot4(0, 0), rcDot4(0, 1), rcDot4(0, 2), rcDot4(0, 3),
            rcDot4(1, 0), rcDot4(1, 1), rcDot4(1, 2), rcDot4(1, 3),
            rcDot4(2, 0), rcDot4(2, 1), rcDot4(2, 2), rcDot4(2, 3),
            rcDot4(3, 0), rcDot4(3, 1), rcDot4(3, 2), rcDot4(3, 3)
        );
        return mat;
    }

    template <class T>
    VectorT4<T>& MatrixT<T>::operator [](int row)
    {
        vector4* rows = (vector4*)ml;
        return rows[row];
    }


    template <class T>
    MatrixT<T>::operator T* () const
    {
        return ml;
    }

    template <class T>
    MatrixT<T>::operator const T* () const
    {
        return ml;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::lookAtLH(MatrixT& out,
                                            const vector3& Eye,
                                            const vector3& At,
                                            const vector3& Up)
    {
        vector3 x, y, z, temp;
        vector3::normalize(z, At - Eye);
        vector3::normalize(x, vector3::cross(temp, Up, z));
        vector3::normalize(y, vector3::cross(temp, z, x));

        out = matrix(
                  x.x, y.x, z.x, 0,
                  x.y, y.y, z.y, 0,
                  x.z, y.z, z.z, 0,
                  -vector3::dot(x, Eye), -vector3::dot(y, Eye), -vector3::dot(z, Eye), 1);

        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::lookAtRH(MatrixT& out,
                                            const vector3& Eye,
                                            const vector3& At,
                                            const vector3& Up)
    {
        vector3 x, y, z, temp;
        vector3::normalize(z, Eye - At);
        vector3::normalize(x, vector3::cross(temp, Up, z));
        vector3::normalize(y, vector3::cross(temp, z, x));

        *out = matrix(
                   x.x, y.x, z.x, 0,
                   x.y, y.y, z.y, 0,
                   x.z, y.z, z.z, 0,
                   -vector3::dot(x, Eye), -vector3::dot(y, Eye), -vector3::dot(z, Eye), 1);

        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::rotationX(MatrixT& out, T angle)
    {
        T s = scalar::sin(angle);
        T c = scalar::cos(angle);

        out = MatrixT(
                  1, 0, 0, 0,
                  0, c, s, 0,
                  0, -s, c, 0,
                  0, 0, 0, 1);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::rotationY(MatrixT& out, T angle)
    {
        T s = scalar::sin(angle);
        T c = scalar::cos(angle);
        out = MatrixT(
                  c, 0, -s, 0,
                  0, 1, 0, 0,
                  s, 0, c, 0,
                  0, 0, 0, 1);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::rotationZ(MatrixT& out, T angle)
    {
        T s = scalar::sin(angle);
        T c = scalar::cos(angle);
        out = MatrixT(
                  c, s, 0, 0,
                  -s, c, 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);
        return out;
    }



    template <class T>
    inline MatrixT<T>& MatrixT<T>::translation(MatrixT& out, const vector3& v)
    {
        out = MatrixT(
                  1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  v.x, v.y, v.z, 1);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::scaling(MatrixT& out, const vector3& v)
    {
        out = MatrixT(
                  v.x, 0, 0, 0,
                  0, v.y, 0, 0,
                  0, 0, v.z, 0,
                  0, 0, 0, 1);

        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::perspectiveFovLH(MatrixT& out, T fovy, T aspect, T zNear, T zFar)
    {
        float y = T(1) / scalar::tan(T(0.5) * fovy);
        float x = y / aspect;

        out = matrix(
                  x, 0.0f, 0.0f, 0.0f,
                  0.0f, y, 0.0f, 0.0f,
                  0.0f, 0.0f, zFar / (zFar - zNear), 1.0f,
                  0.0f, 0.0f, -zFar * zNear / (zFar - zNear), 0.0f);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::perspectiveFovRH(MatrixT& out, T fovy, T aspect, T zNear, T zFar)
    {
        float y = T(1) / scalar::tan(T(0.5) * fovy);
        float x = y / aspect;

        out = matrix(
                  x, 0.0f, 0.0f, 0.0f,
                  0.0f, y, 0.0f, 0.0f,
                  0.0f, 0.0f, zFar / (zFar - zNear), -1.0f,
                  0.0f, 0.0f, zFar * zNear / (zFar - zNear), 0.0f);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::perspectiveOffCenterLH(MatrixT& out, T left, T right, T bottom, T top, T znearPlane, T zfarPlane)
    {
        out = matrix(
                  2 * znearPlane / (right - left), 0, 0, 0,
                  0, 2 * znearPlane / (top - bottom), 0, 0,
                  (left + right) / (left - right), (top + bottom) / (bottom - top), zfarPlane / (zfarPlane - znearPlane), 1,
                  0, 0, znearPlane * zfarPlane / (znearPlane - zfarPlane), 0);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::perspectiveOffCenterRH(MatrixT& out, T left, T right, T bottom, T top, T znearPlane, T zfarPlane)
    {
        out = matrix(
                  2 * znearPlane / (right - left), 0, 0, 0,
                  0, 2 * znearPlane / (top - bottom), 0, 0,
                  (left + right) / (right - left), (top + bottom) / (top - bottom), zfarPlane / (znearPlane - zfarPlane), -1,
                  0, 0, znearPlane * zfarPlane / (znearPlane - zfarPlane), 0);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::orthoLH(MatrixT& out, T w, T h, T zNear, T zFar)
    {
        out = matrix(
                  T(2) / w, 0, 0, 0,
                  0, T(2) / h, 0, 0,
                  0, 0, T(1) / (zFar - zNear), 0,
                  0, 0, zNear / (zNear - zFar), 1);
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::inverse(MatrixT& out, const MatrixT& mat)
    {
        T a9 = mat.m31 * mat.m42 - mat.m32 * mat.m41;
        T aa = mat.m31 * mat.m43 - mat.m33 * mat.m41;
        T a8 = mat.m31 * mat.m44 - mat.m34 * mat.m41;

        T a2 = mat.m32 * mat.m43 - mat.m33 * mat.m42;
        T a5 = mat.m32 * mat.m44 - mat.m34 * mat.m42;
        T a3 = mat.m33 * mat.m44 - mat.m34 * mat.m43;

        T t1 = mat.m22 * a3 - mat.m23 * a5 + mat.m24 * a2;
        T t2 = mat.m21 * a3 - mat.m23 * a8 + mat.m24 * aa;
        T t3 = mat.m21 * a5 - mat.m22 * a8 + mat.m24 * a9;
        T t4 = mat.m21 * a2 - mat.m22 * aa + mat.m23 * a9;

        T det = mat.m11 * t1 - mat.m12 * t2 + mat.m13 * t3 - mat.m14 * t4;

        if (scalar::abs(det) < T(0.00000001))
        {
            out.identity();
            return out;
        }

        det = T(1) / (det);

        T a0 = mat.m21 * mat.m32 - mat.m22 * mat.m31;
        T af = mat.m21 * mat.m42 - mat.m22 * mat.m41;
        T ac = mat.m22 * mat.m33 - mat.m23 * mat.m32;
        T a6 = mat.m22 * mat.m43 - mat.m23 * mat.m42;

        T ab = mat.m23 * mat.m34 - mat.m24 * mat.m33;
        T a7 = mat.m23 * mat.m44 - mat.m24 * mat.m43;
        T ae = mat.m21 * mat.m34 - mat.m24 * mat.m31;
        T ad = mat.m21 * mat.m44 - mat.m24 * mat.m41;

        T b0 = mat.m22 * mat.m34 - mat.m24 * mat.m32;
        T b1 = mat.m21 * mat.m43 - mat.m23 * mat.m41;
        T b2 = mat.m21 * mat.m33 - mat.m23 * mat.m31;
        T b3 = mat.m22 * mat.m44 - mat.m24 * mat.m42;

        out = MatrixT(
                  det * t1,
                  -det * (mat.m12 * a3 - mat.m13 * a5 + mat.m14 * a2),
                  det * (mat.m12 * a7 - mat.m13 * b3 + mat.m14 * a6),
                  -det * (mat.m12 * ab - mat.m13 * b0 + mat.m14 * ac),

                  -det * t2,
                  det * (mat.m11 * a3 - mat.m13 * a8 + mat.m14 * aa),
                  -det * (mat.m11 * a7 - mat.m13 * ad + mat.m14 * b1),
                  det * (mat.m11 * ab - mat.m13 * ae + mat.m14 * b2),

                  det * t3,
                  -det * (mat.m11 * a5 - mat.m12 * a8 + mat.m14 * a9),
                  det * (mat.m11 * b3 - mat.m12 * ad + mat.m14 * af),
                  -det * (mat.m11 * b0 - mat.m12 * ae + mat.m14 * a0),

                  -det * t4,
                  det * (mat.m11 * a2 - mat.m12 * aa + mat.m13 * a9),
                  -det * (mat.m11 * a6 - mat.m12 * b1 + mat.m13 * af),
                  det * (mat.m11 * ac - mat.m12 * b2 + mat.m13 * a0));
        return out;
    }

    template <class T>
    inline MatrixT<T>& MatrixT<T>::transpose(MatrixT& out, const MatrixT& mat)
    {
        vector4* rows = (vector4*)mat.ml;
        out = MatrixT(
                  rows[0].x, rows[1].x, rows[2].x, rows[3].x,
                  rows[0].y, rows[1].y, rows[2].y, rows[3].y,
                  rows[0].z, rows[1].z, rows[2].z, rows[3].z,
                  rows[0].w, rows[1].w, rows[2].w, rows[3].w);
        return out;
    }

    template <class T>
    inline VectorT3<T>& MatrixT<T>::transformVec3(VectorT3<T>& out, const vector3& in, const MatrixT& mat)
    {
        out = vector3(
                  in.x * mat.m11 + in.y * mat.m21 + in.z * mat.m31 + mat.m41,
                  in.x * mat.m12 + in.y * mat.m22 + in.z * mat.m32 + mat.m42,
                  in.x * mat.m13 + in.y * mat.m23 + in.z * mat.m33 + mat.m43);
        return out;
    }

    template <class T>
    inline VectorT2<T>& MatrixT<T>::transformVec2(VectorT2<T>& out, const vector2& in, const MatrixT& mat)
    {
        out = vector2(
                  in.x * mat.m11 + in.y * mat.m21 + mat.m41,
                  in.x * mat.m12 + in.y * mat.m22 + mat.m42);
        return out;
    }

    template <class T>
    inline VectorT4<T>& MatrixT<T>::transformVec4(VectorT4<T>& out, const vector4& in, const MatrixT& mat)
    {
        out = vector4(
                  in.x * mat.m11 + in.y * mat.m21 + in.z * mat.m31 + in.w * mat.m41,
                  in.x * mat.m12 + in.y * mat.m22 + in.z * mat.m32 + in.w * mat.m42,
                  in.x * mat.m13 + in.y * mat.m23 + in.z * mat.m33 + in.w * mat.m43,
                  in.x * mat.m14 + in.y * mat.m24 + in.z * mat.m34 + in.w * mat.m44);
        return out;
    }


    typedef MatrixT<float> Matrix;
    typedef MatrixT<double> MatrixD;

}
