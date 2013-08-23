#pragma once
#include "core/Object.h"
#include "math/Rect.h"

namespace oxygine
{
	DECLARE_SMART(NativeTexture, spNativeTexture);

	struct glyph
	{
		RectF src;

		int ch;
		
		short sw;
		short sh;

		short offset_x;
		short offset_y;	

		short advance_x;
		short advance_y;

		spNativeTexture texture;
	};

	class Font: public ObjectBase
	{
	public:
		Font();
		~Font();

		float getScaleFactor() const {return _scaleFactor;}
		void setScaleFactor(float scale){_scaleFactor = scale;}

		void init(const char *name, int size, int baselineDistance, int lineHeight);
		void addGlyph(const glyph &g);
		void sortGlyphs();

		const glyph*	getGlyph(int code) const;
		int				getBaselineDistance() const;
		int				getSize() const;
		int				getLineHeight() const;

	private:
		typedef vector<glyph> glyphs;
		glyphs _glyphs;		 

		float _scaleFactor;

		int _size;
		int _baselineDistance;
		int _lineHeight;
	};
}