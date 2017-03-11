#pragma once
#include "oxygine-include.h"

#include <math.h>
#include <stdlib.h>

namespace oxygine
{
    template<class T>
    inline T lerp(T a, T b, float v)
    {
        return T(a + (b - a) * v);
    }

    namespace scalar
    {

#define MATH_PI 3.141592653589793f

        inline float randFloat(float start, float size)
        {
            return ((rand() % RAND_MAX) * size) / (float)RAND_MAX + start;
        }

        inline double abs(double s)
        {
#ifdef IW_SYS_MATH_H
            return fabs(s);
#else
            return ::fabs(s);
#endif
        }

        inline float abs(float s)
        {
#ifdef IW_SYS_MATH_H
            return fabsf(s);
#else
            return fabsf(s);
#endif
        }

        inline float sign(float s)
        {return s < 0 ? -1.0f : 1.0f;}

        inline double sign(double s)
        {return s < 0 ? -1.0 : 1.0;}

        inline double sin(double s)
        {return ::sin(s);}

        inline float sin(float s)
        {return ::sinf(s);}

        inline double cos(double s)
        {return ::cos(s);}

        inline float cos(float s)
        {return ::cosf(s);}

        inline double acos(double s)
        {return ::acos(s);}

        inline float acos(float s)
        {return ::acosf(s);}

        inline double tan(double s)
        {return ::tan(s);}

        inline float tan(float s)
        {return ::tanf(s);}

        inline double sqrt(double s)
        {return ::sqrt(s);}

        inline float sqrt(float s)
        {return ::sqrtf(s);}

        inline float sqrt(int s)
        {return ::sqrtf((float)s);}

        inline bool equal(double a, double b, const double eps = 10e-8)
        {
            return ((a < b + eps) && (a > b - eps)) ? true : false;
        }

        inline bool equal(float a, float b, const float eps = (float)(10e-6))
        {
            return ((a < b + eps) && (a > b - eps)) ? true : false;
        }

        template <class T>
        inline T clamp(T v, T min_, T max_)
        {
            if (v < min_)
                v = min_;
            if (v > max_)
                v = max_;
            return v;
        }
    }
}
