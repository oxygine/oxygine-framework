#include "WebImage.h"
#include "../res/ResAnim.h"
#include "../Image.h"
#include "../HttpRequestTask.h"
#include "Sprite.h"
#include <vector>

namespace oxygine
{

    WebImage::WebImage(const WebImage& src, copyOptions opt) : Actor(src, opt)
    {
        _http = 0;
        _image = getChildT<Sprite>("_child_");
    }


    WebImage::WebImage() : _http(0)
    {
        setSize(64, 64);
        _image = new Sprite;
        _image->setName("_child_");
        _image->setTouchEnabled(false);
        addChild(_image);
    }

    WebImage::~WebImage()
    {
    }

    void WebImage::load(const std::string& url)
    {
        _image->setResAnim(0);

        _http = HttpRequestTask::create();
        if (!_http)
            return;
        _http->setUrl(url);
        _http->run();

        addRef();//protect actor for delete
        _http->addEventListener(AsyncTask::COMPLETE, CLOSURE(this, &WebImage::loaded));
        _http->addEventListener(AsyncTask::ERROR, CLOSURE(this, &WebImage::error));
    }

    void WebImage::error(Event* e)
    {
        dispatchEvent(e);
        releaseRef();
        _http = 0;
    }

    void WebImage::loaded(Event* e)
    {
        if (_ref_counter <= 1)//if it is already dead
        {
            releaseRef();
            return;
        }


        dispatchEvent(e);

        file::buffer bf;
        _http->getResponseSwap(bf.data);

        Image mt;
        if (mt.init(bf, true))
        {
            _rs.init(&mt);
            _image->setResAnim(&_rs);

            fit();
        }

        _http = 0;
        releaseRef();
    }

    void WebImage::unload()
    {
        _image->setResAnim(0);
    }

    void WebImage::fit()
    {
        if (!_rs.getTotalFrames())
            return;
        if (!_rs.getWidth())
            return;
        float sx = getWidth() / _rs.getWidth();
        float sy = getHeight() / _rs.getHeight();
        float s = std::min(sx, sy);
        _image->setScale(s);
    }

    void WebImage::sizeChanged(const Vector2& size)
    {
        fit();
    }
}