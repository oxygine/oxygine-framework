#include "SingleResAnim.h"
#include "../Image.h"
#include "../core/VideoDriver.h"
#include "../core/NativeTexture.h"
namespace oxygine
{
    void makeAlpha(const ImageData& srcImage, Rect& bounds, std::vector<unsigned char>& alpha, HitTestData& adata, bool hittest);


    void SingleResAnim::init(const std::string& file, int columns /* = 1 */, int rows /* = 1 */, float scaleFactor /* = 1.0f */)
    {
        ResAnim::init(file, columns, rows, scaleFactor);
    }

    void SingleResAnim::init(Image* original, int columns, int rows, float scaleFactor)
    {
        _scaleFactor = scaleFactor;
        if (!original)
            return;

        spNativeTexture texture = IVideoDriver::instance->createTexture();
        _texture = texture;

        texture->init(original->lock(), false);
        texture->apply();
        ResAnim::init(texture, original->getSize(), columns, rows, scaleFactor);



        Point originalSize = original->getSize();

        int frame_width = originalSize.x / columns;
        int frame_height = originalSize.y / rows;

        Vector2 frameSize((float)frame_width, (float)frame_height);
        int i = 0;
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                Rect src;
                src.pos = Point(x * frame_width, y * frame_height);
                src.size = Point(frame_width, frame_height);


                Rect bounds;
                HitTestData ht;
                const ImageData& im = original->lock(src);
                makeAlpha(im, bounds, _data, ht, true);

                _frames[i].setHitTestData(ht);
                ++i;
            }
        }

        i = 0;
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                HitTestData ht = _frames[i].getHitTestData();
                ht.data = &_data[reinterpret_cast<size_t>(ht.data)];
                _frames[i].setHitTestData(ht);
                ++i;
            }
        }

    }

    spNativeTexture SingleResAnim::getTexture() const
    {
        return _texture;
    }

}
