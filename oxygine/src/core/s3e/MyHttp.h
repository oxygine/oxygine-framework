#pragma once
#include "core/oxygine.h"
#include "closure/closure.h"
#include "core/file.h"

#include "IwHTTP.h"

class MyHttp
{
public:
    typedef Closure<void (MyHttp*)> DownloadDoneCallback;
    typedef Closure<void (MyHttp*)> ErrorCallback;
    typedef Closure<void (MyHttp*, int)> ProgressCallback;

    enum status
    {
        status_none,
        status_inprogress,
        status_done,
        status_error
    };

    MyHttp();
    virtual ~MyHttp();


    DownloadDoneCallback    _cbDone;
    ProgressCallback        _cbProgress;
    ErrorCallback           _cbError;

    void            getFile(const std::string& url, const std::string& name);
    void            get(const std::string& url);
    void            post(const std::string& url, const char* data, int size);


    std::vector<unsigned char>&  getBuffer() {return _data;}
    status          getStatus() const {return _status;}
    unsigned int    getTotalSize() const;
    unsigned int    getReceivedSize() const;
    int             getResponseCode() const;

private:

    enum METHOD {GETFILE, GET, POST};
    METHOD _method;
    static int _gotHeaders(void* systemData, void* userData);
    static int _gotData(void* systemData, void* userData);
    static int _delayedError(void* systemData, void* userData);


    void destroy();

    void gotHeaders();
    void gotData(int size);
    void progress(int size);

    void onError();

    std::string _url;
    const char* _post;
    int _postSize;

    std::vector<unsigned char> _tempBuffer;
    std::vector<unsigned char> _data;
    oxygine::file::handle _handle;
    CIwHTTP* _http;

    status _status;
};


void makeSingleHttpAsyncGetRequest(const char* url);
void makeSingleHttpAsyncPostRequest(const char* url, const char* data, int size);