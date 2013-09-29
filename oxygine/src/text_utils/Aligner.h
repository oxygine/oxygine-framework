#pragma once
#include "oxygine_include.h"
#include <vector>
#include "math/Rect.h"
#include "math/Color.h"
#include "TextStyle.h"

namespace oxygine
{
	struct glyph;
	class Font;

	namespace text
	{
		using namespace std;

		struct Symbol
		{
			Symbol():gl(0), x(0), y(0), code(0){}

			short x, y;
			int code;
			const glyph *gl;
			RectF destRect;
		};

		class Aligner
		{
		public:
			Aligner();
			~Aligner();

			
			const TextStyle &getStyle() const {return style;}
			float getScaleFactor() const;

			void begin();
			void end();		
		
			int putSymbol(Symbol &s);		
			void nextLine();		


			TextStyle style;
			Rect bounds;		
			int width;
			int height;

		private:		
			int getLineWidth()const;
			int getLineSkip()const;

			typedef vector<Symbol*> line;

			void _alignLine(line &ln);
			int _alignX(int rx);
			int _alignY(int ry);
			void _nextLine(line &ln);		

			int _x, _y;		
			line _line;
			int _lineWidth;
		};
	}
}