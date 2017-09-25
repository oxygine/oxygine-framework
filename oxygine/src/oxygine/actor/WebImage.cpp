#include "WebImage.h"
#include "../res/ResAnim.h"
#include "../Image.h"
#include "../HttpRequestTask.h"
#include "Sprite.h"
#include "../Serialize.h"
#include <vector>

namespace oxygine
{
    void WebImage::copyFrom(const WebImage& src, cloneOptions opt)
    {
        inherited::copyFrom(src, opt);
        _http = 0;
        _image = getChildT<Sprite>("_child_");
    }


    WebImage::WebImage() : _http(0), _allowSwap(false)
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
        _allowSwap = true;
        _image->setResAnim(0);

        spHttpRequestTask task = HttpRequestTask::create();
        if (!task)
            return;
        task->setUrl(url);
        task->run();

        _load(task);
    }

    void WebImage::load(spHttpRequestTask task)
    {
        _allowSwap = false;
        _image->setResAnim(0);
        _load(task);
    }

    void WebImage::_load(spHttpRequestTask task)
    {
        _http = task;

        if (task->getStatus() == HttpRequestTask::status_completed)
        {
            init();
            return;
        }

        if (task->getStatus() == HttpRequestTask::status_failed)
        {
            return;
        }


        addRef();//protect actor from delete
        _http->addEventListener(AsyncTask::COMPLETE, CLOSURE(this, &WebImage::loaded));
        _http->addEventListener(AsyncTask::ERROR, CLOSURE(this, &WebImage::error));
    }

    void WebImage::error(Event* e)
    {
        dispatchEvent(e);
        releaseRef();
        _http = 0;
    }

    void WebImage::init()
    {
        file::buffer bf;
        if (_allowSwap)
        {
            _http->getResponseSwap(bf.data);
            _allowSwap = false;
        }
        else
        {
            bf.data = _http->getResponse();
        }

        Image mt;
        if (mt.init(bf, true))
        {
            _rs.init(&mt);
            _image->setResAnim(&_rs);

            fit();
        }
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

        init();
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

    void WebImage::serialize(serializedata* data)
    {
        inherited::serialize(data);
        data->node.set_name("WebImage");
    }

    void WebImage::deserialize(const deserializedata* data)
    {
        inherited::deserialize(data);
    }
}
