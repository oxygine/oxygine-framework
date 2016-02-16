#pragma once
#include "test.h"
#include "ClipRectActor.h"
#include "WebImage.h"
#include "ProgressBar.h"
#include "HttpRequestTask.h"

class TestHttp: public Test
{
public:
    spProgressBar _bar;

    TestHttp()
    {
        //Testing http GET request
        spWebImage image1 = new WebImage;
        image1->load("http://oxygine.org/img/madewith.png");
        image1->setSize(100, 100);
        image1->setPosition(image1->getPosition() + Vector2(image1->getWidth() + 10, 0));
        image1->attachTo(content);

        //Testing http GET request with redirect
        spWebImage image2 = new WebImage;
        image2->load("http://graph.facebook.com/1373973107/picture?type=normal&return_ssl_resources=0");
        image2->setSize(100, 100);
        image2->setPosition(getStage()->getSize() / 2);
        image2->attachTo(content);

        //Testing loading a file in memory
        spHttpRequestTask task = HttpRequestTask::create();
        task->setUrl("http://nist.time.gov/actualtime.cgi");
        task->addEventListener(HttpRequestTask::COMPLETE, CLOSURE(this, &TestHttp::dateTimeLoaded));
        task->run();

        //Testing downloading a file to disk
        task = HttpRequestTask::create();
        task->setUrl("http://oxygine.org/emscripten/MPHello.js.gz");
        task->setFileName("somefile.abc");
        task->addEventListener(HttpRequestTask::COMPLETE, CLOSURE(this, &TestHttp::loaded));
        task->addEventListener(HttpRequestTask::PROGRESS, CLOSURE(this, &TestHttp::progress));
        task->run();

        //OX_ASSERT(!file::exists("somefile.abc"));
        file::deleteFile("somefile.abc", ep_ignore_error);

        _bar = new ProgressBar;
        _bar->setResAnim(resources.getResAnim("slide"));
        _bar->setProgress(0);
        _bar->setY(getStage()->getHeight() - _bar->getHeight());
        _bar->attachTo(content);

        //Testing http POST
        task = HttpRequestTask::create();
        task->setUrl("http://oxygine.org/test/reply.php");
        vector<unsigned char> postBody;
        for (int i = 0; i < 256; ++i)
            postBody.push_back(i);
        task->setPostData(postBody);
        task->addEventListener(HttpRequestTask::COMPLETE, CLOSURE(this, &TestHttp::onPostLoaded));
        task->run();

//fix conflict with windows.h
#undef ERROR

        task = HttpRequestTask::create();
        task->setUrl("http://oxygine.org/test/missing.php");
        task->addEventListener(HttpRequestTask::ERROR, CLOSURE(this, &TestHttp::onError));
        task->run();
    }

    void progress(Event* event)
    {
        HttpRequestTask::ProgressEvent* ev = safeCast<HttpRequestTask::ProgressEvent*>(event);

        float p = ev->loaded / (float)ev->total;
        _bar->setProgress(p);
    }

    void onError(Event* event)
    {
        HttpRequestTask* task = safeCast<HttpRequestTask*>(event->currentTarget.get());
        //OX_ASSERT(task->_ref_counter == 2);
    }

    void onPostLoaded(Event* event)
    {
        notify("onPostLoaded");

        spHttpRequestTask task = safeCast<HttpRequestTask*>(event->currentTarget.get());
        std::vector<unsigned char> res = task->getResponse();
        OX_ASSERT(res.size() == 256);
        for (size_t i = 0; i < 256; ++i)
        {
            OX_ASSERT(res[i] == i);
        }
    }

    void dateTimeLoaded(Event* event)
    {
        HttpRequestTask* task = safeCast<HttpRequestTask*>(event->currentTarget.get());
        const std::vector<unsigned char>& response = task->getResponse();

        pugi::xml_document doc;
        doc.load_buffer(&response.front(), response.size());
        string time = doc.root().first_child().attribute("time").as_string();
        notify("internet time:\n" + time, 2000);

    }

    void loaded(Event* event)
    {
        //OX_ASSERT(file::exists("somefile.abc"));

        if (file::exists("somefile.abc"))
        {
            notify("file loaded");
        }
    }
};
