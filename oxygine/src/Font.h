#pragma once
#include "oxygine_include.h"
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

		void init(const char *name, int size, int baselineDistance, int lineHeight);

		void addGlyph(const glyph &g);
		void sortGlyphs();

		void setScale(float scale){ _scale = scale; }

		const glyph*	getGlyph(int code) const;
		int				getBaselineDistance() const;
		int				getSize() const;
		float			getScale() const;
		int				getLineHeight() const;

	private:
		typedef std::vector<glyph> glyphs;
		glyphs _glyphs;		 

		float _scale;

		int _size;		
		int _baselineDistance;
		int _lineHeight;
	};
}