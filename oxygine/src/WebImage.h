#pragma once
#include "oxygine-include.h"
#include "Actor.h"
#include "res/ResAnim.h"
#include <string>

namespace oxygine
{
    DECLARE_SMART(WebImage, spWebImage);
    class WebImage : public Actor
    {
        INHERITED(Actor);
    public:
        DECLARE_COPYCLONE(WebImage);

        WebImage();
        ~WebImage();

        void load(const std::string& url);
        void load(spHttpRequestTask task);
        void unload();

        void serialize(serializedata*) override;
        void deserialize(const deserializedata*) override;

    private:
        void _load(spHttpRequestTask task);

        void loaded(Event*);
        void error(Event*);
        void sizeChanged(const Vector2& size) override;
        void fit();

        void init();

        ResAnim _rs;

        spSprite _image;

        spHttpRequestTask _http;
        bool _allowSwap;
    };
}

EDITOR_INCLUDE(WebImage);