#pragma once
#include "oxygine-include.h"
#include "Actor.h"
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

        spSprite _image;

        spHttpRequestTask _http;
    };
}

EDITOR_INCLUDE(WebImage);