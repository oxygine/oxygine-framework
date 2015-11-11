#include "WebImage.h"
#include "res/ResAnim.h"
#include "MemoryTexture.h"
#include "HttpRequestTask.h"
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
        dispatchEvent(e);

        file::buffer bf;
        _http->getResponseSwap(bf.data);

        MemoryTexture mt;
        if (mt.init(bf, true))
        {
            ResAnim rs;
            rs.init(&mt);
            float w = (float)rs.getFrame(0, 0).getWidth();
            if (w != 0.0f)
                _image->setScale(getWidth() / w);
            _image->setAnimFrame(&rs);
        }

        _http = 0;
        releaseRef();
    }

    void WebImage::unload()
    {
        _image->setResAnim(0);
    }
}