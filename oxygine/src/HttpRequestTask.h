#pragma once
#include "oxygine-include.h"
#include "AsyncTask.h"
#include <vector>
#include <string>

namespace oxygine
{
    DECLARE_SMART(HttpRequestTask, spHttpRequestTask);
    class HttpRequestTask: public AsyncTask
    {
    public:
        static spHttpRequestTask create();
        typedef HttpRequestTask* (*createHttpRequestCallback)();
        static void setCustomRequests(createHttpRequestCallback);
        static void init();
        static void release();


        /**dispatching AsyncTask events*/
        enum
        {
            ERROR = AsyncTask::ERROR,
            PROGRESS = AsyncTask::PROGRESS,
            COMPLETE = AsyncTask::COMPLETE,
        };

        class ProgressEvent : public Event
        {
        public:
            enum {EVENT = PROGRESS};
            ProgressEvent(int Delta, int Loaded, int Total) : Event(PROGRESS), delta(Delta), loaded(Loaded), total(Total) {};

            int delta;
            int loaded;
            int total;
        };

        HttpRequestTask();
        ~HttpRequestTask();

        const std::vector<unsigned char>&   getResponse() const;
        const std::vector<unsigned char>&   getPostData() const;
        const std::string&                  getFileName() const;


        /**swap version of getResponse if you want to modify result buffer inplace*/
        void getResponseSwap(std::vector<unsigned char>&);
        int  getResponseCode() const { return _responseCode; }
        void addHeader(const std::string& key, const std::string& value);

        void setPostData(const std::vector<unsigned char>& data);
        void setUrl(const std::string& url);
        void setFileName(const std::string& name);
        void setCacheEnabled(bool enabled);
        void setSuccessOnAnyResponseCode(bool en) { _successOnAnyResponceCode = en; }

    protected:
        void _prerun();
        void _onError() override;
        void _onComplete() override;
        void _dispatchComplete() override;

        //async
        void progress(int loaded, int total);

        void _onCustom(const ThreadDispatcher::message& msg);
        void dispatchProgress(int loaded, int total);

        virtual void _setFileName(const std::string& name) {}
        virtual void _setUrl(const std::string& url) {}
        virtual void _setPostData(const std::vector<unsigned char>& data) {}
        virtual void _setCacheEnabled(bool enabled) {}
        virtual void _addHeader(const std::string& key, const std::string& value) {}

        int _loaded;
        std::string _url;
        std::string _fname;
        bool _cacheEnabled;
        std::vector<unsigned char> _response;
        std::vector<unsigned char> _postData;

        int _responseCode;
        bool _successOnAnyResponceCode;

        typedef std::vector< std::pair<std::string, std::string> >  headers;
        headers _headers;
    };
}