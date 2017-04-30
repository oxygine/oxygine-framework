#pragma once
#include "oxygine-include.h"
#include "Actor.h"
#include "../res/ResAnim.h"
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
        void unload();

    private:
        void loaded(Event*);
        void error(Event*);
        void sizeChanged(const Vector2& size) override;
        void fit();

        ResAnim _rs;

        spSprite _image;

        spHttpRequestTask _http;
    };
}

EDITOR_INCLUDE(WebImage);