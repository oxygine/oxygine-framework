#include "SingleResAnim.h"
#include "Image.h"

namespace oxygine
{
	void makeAlpha(const ImageData& srcImage, Rect& bounds, std::vector<unsigned char>& alpha, HitTestData& adata, bool hittest);

	void SingleResAnim::init(const std::string& file, int columns, int rows, float scaleFactor)
	{

	}

	void SingleResAnim::init(Image* original, int columns, int rows, float scaleFactor)
	{
		Rect bounds;
		HitTestData ht;
		const ImageData &im = original->lock();
		makeAlpha(im, bounds, _data, ht, true);
		int q = 0;
	}
}
