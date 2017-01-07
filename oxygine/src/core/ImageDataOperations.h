#pragma once
#include "oxygine-include.h"
#include "ImageData.h"

namespace oxygine
{
    namespace operations
    {
        //based on memcpy
        void copy(const ImageData& src, const ImageData& dest);

        //based on memmove, could be used for overlapped images, slower than copy
        void move(const ImageData& src, const ImageData& dest);

        void blit(const ImageData& src, const ImageData& dest);
        void blitColored(const ImageData& src, const ImageData& dest, const Color& c);
        void blitPremultiply(const ImageData& src, const ImageData& dest);
        void premultiply(const ImageData& dest);
        void flipY(const ImageData& src, const ImageData& dest);
        void blend(const ImageData& src, const ImageData& dest);

        inline void blend_srcAlpha_invSrcAlpha(const Pixel& pS, Pixel& pD)
        {
            const unsigned int& s = pS.rgba;
            unsigned int& d = pD.rgba;

            unsigned int dst_rb = d        & 0xFF00FF;
            unsigned int dst_ag = (d >> 8) & 0xFF00FF;

            unsigned int src_rb = s        & 0xFF00FF;
            unsigned int src_ag = (s >> 8) & 0xFF00FF;

            unsigned int d_rb = src_rb - dst_rb;
            unsigned int d_ag = src_ag - dst_ag;

            d_rb *= pS.a;
            d_ag *= pS.a;
            d_rb >>= 8;
            d_ag >>= 8;

            const unsigned int rb  = (d_rb + dst_rb)        & 0x00FF00FF;
            const unsigned int ag  = ((d_ag + dst_ag) << 8) & 0xFF00FF00;

            d = rb | ag;
        }


        template <class Op>
        void applyOperation(const Op& op, const ImageData& src, const ImageData& dest);


        class op_fill
        {
        public:
            op_fill() {color.rgba = 0xffffffff;}

            Pixel color;

            template<class Src, class Dest>
            void operator()(const Src& s, Dest& d, const unsigned char* srcData, unsigned char* destData) const
            {
                d.setPixel(destData, color);
            }
        };

        class op_noise
        {
        public:
            op_noise(int v): _v(v) {}

            template<class Src, class Dest>
            void operator()(const Src& srcPixelFormat, Dest& destPixelFormat, const unsigned char* srcData, unsigned char* destData) const
            {
                Pixel p;

                p.r = 255;
                p.g = 255;
                p.b = 255;

                int v = rand() % 1000;
                //p.r = p.g = p.b = p.a = v > 600 ? 255:0;//for add
                p.r = p.g = p.b = p.a = v > _v ? 255 : 0; //for alpha


                destPixelFormat.setPixel(destData, p);
            }

            int _v;
        };

        class op_premultipliedAlpha
        {
        public:
            template<class Src, class Dest>
            void operator()(const Src& srcPixelFormat, Dest& destPixelFormat, const unsigned char* srcData, unsigned char* destData) const
            {
                Pixel p;
                srcPixelFormat.getPixel(srcData, p);

                //we need correct "snapped" to pixel format alpha
                unsigned char na = destPixelFormat.snap_a(p.a);

                p.r = (p.r * na) / 255;
                p.g = (p.g * na) / 255;
                p.b = (p.b * na) / 255;

                destPixelFormat.setPixel(destData, p);
            }
        };

        class op_blit
        {
        public:
            template<class Src, class Dest>
            void operator()(const Src& srcPixelFormat, Dest& destPixelFormat, const unsigned char* srcData, unsigned char* destData) const
            {
                Pixel p;
                srcPixelFormat.getPixel(srcData, p);
                destPixelFormat.setPixel(destData, p);
            }
        };

        class op_blit_colored
        {
        public:
            op_blit_colored(const Pixel& clr): color(clr) {}

            template<class Src, class Dest>
            void operator()(const Src& srcPixelFormat, Dest& destPixelFormat, const unsigned char* srcData, unsigned char* destData) const
            {
                Pixel src;
                srcPixelFormat.getPixel(srcData, src);

                Pixel dest;
                dest.r = (src.r * color.r) / 255;
                dest.g = (src.g * color.g) / 255;
                dest.b = (src.b * color.b) / 255;
                dest.a = (src.a * color.a) / 255;

                destPixelFormat.setPixel(destData, dest);
            }

            Pixel color;
        };



        class op_blend_one_invSrcAlpha
        {
        public:
            template<class Src, class Dest>
            void operator()(const Src& srcPixelFormat, Dest& destPixelFormat, const unsigned char* srcData, unsigned char* destData) const
            {
                Pixel s;
                srcPixelFormat.getPixel(srcData, s);

                Pixel d;
                destPixelFormat.getPixel(destData, d);

#define M(v) std::min(v, 255);
                unsigned char a = s.a;
                unsigned char ia = 255 - a;
                Pixel r;
                r.r = M((d.r * ia) / 255 + (s.r * a) / 255);
                r.g = M((d.g * ia) / 255 + (s.g * a) / 255);
                r.b = M((d.b * ia) / 255 + (s.b * a) / 255);
                r.a = M((d.a * ia) / 255 + (s.a * a) / 255);
#undef  M

                destPixelFormat.setPixel(destData, r);
            }
        };


        class op_blend_srcAlpha_invSrcAlpha
        {
        public:
            template<class Src, class Dest>
            void operator()(const Src& srcPixelFormat, Dest& destPixelFormat, const unsigned char* srcData, unsigned char* destData) const
            {
                Pixel pS;
                srcPixelFormat.getPixel(srcData, pS);

                Pixel pD;
                destPixelFormat.getPixel(destData, pD);

                blend_srcAlpha_invSrcAlpha(pS, pD);
                destPixelFormat.setPixel(destData, pD);
            }
        };


        bool check(const ImageData& src, const ImageData& dest);


        template <class Op, class Src, class Dest>
        void applyOperationT(const Op& op, const Src& srcPixelFormat, Dest& destPixelFormat, const ImageData& src, const ImageData& dest)
        {
            if (!check(src, dest))
                return;

            const unsigned char* srcBuffer = (unsigned char*)src.data;
            unsigned char* destBuffer = (unsigned char*)dest.data;

            int w = dest.w;
            int h = dest.h;

            for (int y = 0; y != h; ++y)
            {
                const unsigned char* srcLine = srcBuffer;
                unsigned char* destLine = destBuffer;

                for (int x = 0; x != w; ++x)
                {
                    op(srcPixelFormat, destPixelFormat, srcLine, destLine);

                    destLine += dest.bytespp;
                    srcLine += src.bytespp;
                }

                srcBuffer += src.pitch;
                destBuffer += dest.pitch;
            }
        }


        template <class Op, class Dest>
        void applyOperationT(const Op& op, Dest& destPixelFormat, const ImageData& dest)
        {
            if (!check(dest, dest))
                return;

            unsigned char* destBuffer = (unsigned char*)dest.data;

            int w = dest.w;
            int h = dest.h;

            for (int y = 0; y != h; ++y)
            {
                unsigned char* destLine = destBuffer;

                for (int x = 0; x != w; ++x)
                {
                    op(destPixelFormat, destPixelFormat, destLine, destLine);
                    destLine += dest.bytespp;
                }

                destBuffer += dest.pitch;
            }
        }




#define FORMAT_OP1(format) case TF_##format: \
        { \
            Pixel##format d; \
            applyOperationT(op, s, d, src, dest); \
        } \
        break;

        template<class Src, class Op>
        void SwitchSrcDestT(const Op& op, const Src& s, const ImageData& src, const ImageData& dest)
        {
#define FORMAT_CASE FORMAT_OP1
            ALL_FORMATS_SWITCH(dest.format);
#undef FORMAT_CASE
        }


#define FORMAT_OP2(format) case TF_##format: \
        { \
            Pixel##format s; \
            SwitchSrcDestT(op, s, src, dest); \
        } \
        break;


        template <class Op>
        void applyOperation(const Op& op, const ImageData& src, const ImageData& dest)
        {
#define FORMAT_CASE FORMAT_OP2
            ALL_FORMATS_SWITCH(src.format);
#undef FORMAT_CASE
        }


#define FORMAT_OP3(format) case TF_##format: \
        { \
            Pixel##format d; \
            applyOperationT(op, d, dest); \
        } \
        break;

        template <class Op>
        void applyOperation(const Op& op, const ImageData& dest)
        {
#define FORMAT_CASE FORMAT_OP3
            ALL_FORMATS_SWITCH(dest.format);
#undef FORMAT_CASE
        }
    }
}