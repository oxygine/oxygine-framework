#pragma once
#include "test.h"
#include "ThreadLoader.h"
#include "pthread.h"
#include "core/oxygine.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


void* myThreadFunc(void* t)
{
    Test* test = (Test*)t;
    resources.load();

#ifndef __S3E__

    //sync with game thread and show notification
    core::getMainThreadDispatcher().postCallback([ = ]()
    {
        test->notify("loaded");
    });

#endif

    return 0;
}


class ManageResTest: public Test
{
public:
    ManageResTest()
    {
        toggle sw[] = {toggle("unload resources", 1), toggle("load resources", 0)};
        addToggle("switch", sw, 2);

#ifdef EMSCRIPTEN
#else
        addButton("mt1", "load using ThreadLoader");
        addButton("mt2", "load from own thread");
#endif


        Resources::resources items;
        resources.collect(items);

        for (size_t i = 0; i < items.size(); ++i)
        {
            ResAnim* ra = dynamic_cast<ResAnim*>(items[i].get());
            if (!ra)
                continue;
            if (ra->getName().find("_") != string::npos)
                continue;
            spSprite sprite = new Sprite;
            sprite->setResAnim(ra);
            if (ra->getTotalFrames() > 1)
                sprite->addTween(TweenAnim(ra), 500, -1);
            sprite->setPosition(scalar::randFloat(50.0f, getWidth() - 100.0f), scalar::randFloat(50.0f, getHeight() - 100.0f));
            sprite->attachTo(content);

            if (ra->isName("bg"))
            {
                sprite->setPosition(0, 0);
                sprite->setPriority(-1);
            }
        }

        spTextField text = new TextField;
        text->attachTo(content);

        text->setSize(300, 200);
        text->setPosition(140.0f, (float)getHeight() - text->getHeight());

        TextStyle st;
        st.font = resources.getResFont("font");
        st.vAlign = TextStyle::VALIGN_TOP;
        st.color = Color::CornflowerBlue;
        st.multiline = true;
        text->setStyle(st);
        text->setText("The quick brown fox jumps over the lazy dog. 1234567890");
        //text->addEventHandler(new DragHandler);
    }

    void _loaded(Event* event)
    {
        notify("Loaded!");
        ui->getChild("loading")->addTween(Sprite::TweenAlpha(0), 400)->setDetachActor(true);

        releaseRef();//added ref earlier from void clicked(id)
    }

    void toggleClicked(string id, const toggle* data)
    {
        if (id == "switch")
        {
            if (data->value)
                resources.unload();
            else
                resources.load();
        }
    }

    spSprite createLoadingAnimation()
    {
        spSprite sp = new Sprite;
        sp->setName("loading");
        sp->setResAnim(resourcesUI.getResAnim("loading"));
        sp->attachTo(ui);
        sp->setAnchor(0.5f, 0.5f);
        sp->setPosition(getSize() - sp->getSize() / 4);
        sp->setScale(0.5f);
        sp->addTween(Actor::TweenRotation(-(float)MATH_PI * 2), 1500, -1);

        return sp;
    }

    void clicked(string id)
    {
        if (id == "mt1")
        {
            resources.unload();

            createLoadingAnimation();

            spThreadLoader loading = new ThreadLoader;

            loading->addEventListener(ThreadLoader::COMPLETE, CLOSURE(this, &ManageResTest::_loaded));
            addRef();//protect Test instance from automatic delete if you close it too fast

            loading->add(&resources);
            loading->start();
        }

        if (id == "mt2")
        {
            resources.unload();

            pthread_t thread;
            pthread_create(&thread, 0, myThreadFunc, this);
        }
    }
};
