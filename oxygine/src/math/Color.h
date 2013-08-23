#pragma once
#include "ScalarMath.h"

namespace oxygine
{
	class Color
	{
	public:
		Color():argb(0xffffffff){}
		explicit Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		{
			this->a = a;
			this->r = r;
			this->g = g;
			this->b = b;
		}
		explicit Color(unsigned int rgba)
		{
			r = rgba >> 24;
			g = (rgba >> 16) & 0xFF;
			b = (rgba >> 8) & 0xFF;
			a = rgba & 0xFF;
		}



		unsigned char operator [](int i) const
		{
			return colors[i];
		}


		unsigned int rgba() const
		{
			unsigned int c = 0;
			unsigned char *p = (unsigned char *)(&c);
			p[0] = r;
			p[1] = g;
			p[2] = b;
			p[3] = a;
			return c;
		}

		static Color interpolate(const Color &a, const Color &b, float v)
		{
			Color c;
			for (int i = 0; i < 4; ++i)
				c.colors[i] = oxygine::interpolate(a.colors[i], b.colors[i], v);
			
			return c;
		}

		float getRedF() const{return r / 255.0f;}
		float getGreenF() const{return g / 255.0f;}
		float getBlueF() const{return b / 255.0f;}
		float getAlphaF() const{return a / 255.0f;}

		Color operator + (const Color &c) const
		{			
			Color v(
				scalar::clamp(r + c.r, 0, 255), 
				scalar::clamp(g + c.g, 0, 255), 
				scalar::clamp(b + c.b, 0, 255), 
				scalar::clamp(a + c.a, 0, 255));
			return v;
		}

		Color operator - (const Color &c) const
		{			
			Color v(
				scalar::clamp(r - c.r, 0, 255), 
				scalar::clamp(g - c.g, 0, 255), 
				scalar::clamp(b - c.b, 0, 255), 
				scalar::clamp(a - c.a, 0, 255));
			return v;
		}


		Color operator * (const Color &c) const
		{
			Color v(
				r * c.r / 255, 
				g * c.g / 255, 
				b * c.b / 255, 
				a * c.a / 255);
			return v;
		}
		/*
		Color operator / (const Color &c) const
		{
			Color cc = c;
			const int v = 1;
			if (cc.r == 0)
				cc.r = v;
			if (cc.g == 0)
				cc.g = v;
			if (cc.b == 0)
				cc.b = v;
			if (cc.a == 0)
				cc.a = v;
			Color r(
				r * 255 / cc.r,
				g * 255 / cc.g,
				b * 255 / cc.b, 
				a * 255 / cc.a);
			return r;
		}
		*/

		Color premultiplied() const
		{
			Color c = *this;
			c.r = (c.r * a)/255;
			c.g = (c.g * a)/255;
			c.b = (c.b * a)/255;			

			return c;
		}

		bool operator == (const Color &color) const
		{
			return argb == color.argb;
		}

		bool operator != (const Color &color) const
		{
			return argb != color.argb;
		}

		union
		{
			struct
			{
				unsigned char a, r, g, b;
			};			
			unsigned int argb;
			unsigned char colors[4];
		};
	};

	inline Color interpolate(const Color &a, const Color &b, float v)
	{
		return Color::interpolate(a, b, v);
	}
}