#pragma once
#include "oxygine-include.h"
#include "AsyncTask.h"
#include <vector>
#include <string>
#include "core/file.h"

namespace oxygine
{
    DECLARE_SMART(HttpRequestTask, spHttpRequestTask);
    class HttpRequestTask: public AsyncTask
    {
    public:
        static spHttpRequestTask create();
        typedef HttpRequestTask* (*createHttpRequestCallback)();
        typedef std::function< bool(int) > responseCodeChecker;
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
            ProgressEvent(size_t Delta, size_t Loaded, size_t Total) : Event(PROGRESS), delta(Delta), loaded(Loaded), total(Total) {};

            size_t delta;
            size_t loaded;
            size_t total;
        };

        HttpRequestTask();
        ~HttpRequestTask();

        const std::vector<unsigned char>&   getResponse() const;
        const std::vector<unsigned char>&   getPostData() const;
        const std::string&                  getFileName() const;
        const std::string&                  getUrl() const;


        /**swap version of getResponse if you want to modify result buffer inplace*/
        void getResponseSwap(std::vector<unsigned char>&);
        int  getResponseCode() const { return _responseCode; }
        const responseCodeChecker& getResponseCodeChecker() const {return _responseCodeChecker;}
        void addHeader(const std::string& key, const std::string& value);

        void setPostData(const std::vector<unsigned char>& data);
        void setUrl(const std::string& url);
        void setFileName(const std::string& name, bool continueDownload = false);
        void setCacheEnabled(bool enabled);


        void setResponseCodeChecker(const responseCodeChecker& f) {_responseCodeChecker = f;}
        void setSuccessOnAnyResponseCode(bool en);

    protected:
        bool _prerun() override;
        void _onError() override;
        void _onComplete() override;
        void _dispatchComplete() override;
        void _finalize(bool error) override;

        std::string _getRunInfo() const override { return _url; }

        void gotHeaders();
        void write(const void* data, size_t size);

        //async
        void asyncProgress(size_t delta, size_t loaded, size_t total);

        void dispatchProgress(size_t delta, size_t loaded, size_t total);

        virtual void _setFileName(const std::string& name) {}
        virtual void _setUrl(const std::string& url) {}
        virtual void _setPostData(const std::vector<unsigned char>& data) {}
        virtual void _setCacheEnabled(bool enabled) {}
        virtual void _addHeader(const std::string& key, const std::string& value) {}

        std::string _url;
        std::string _fname;
        file::handle _fhandle;
        bool _writeFileError;
        bool _cacheEnabled;

        bool _progressDispatched;
        unsigned int _progressDeltaDelayed;

        std::vector<unsigned char> _response;
        std::vector<unsigned char> _postData;

        responseCodeChecker _responseCodeChecker;

        bool _suitableResponse;

        int _responseCode;
        size_t _expectedContentSize;
        size_t _receivedContentSize;

        bool _continueDownload;

        typedef std::vector< std::pair<std::string, std::string> >  headers;
        headers _headers;
    };
}
