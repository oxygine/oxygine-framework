#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#include "HttpRequestCocoaTask.h"
#include "file.h"
#include "STDFileSystem.h"
#include "core/log.h"
using namespace oxygine;
static char taskKey;


static HttpRequestTask *createTask()
{
    return new HttpRequestCocoaTask;
}

@interface HttpRequests:NSObject<NSURLSessionTaskDelegate, NSURLSessionDataDelegate>
{
}
@end

@implementation HttpRequests

- (id)init
{
    if (!(self = [super init]))
        return nil;
    
    return self;
}

-(oxygine::HttpRequestCocoaTask*) getTask:(NSObject*)object remove:(BOOL)remove
{
    NSValue *taskValue = objc_getAssociatedObject(object, &taskKey);
    oxygine::HttpRequestCocoaTask* task = (oxygine::HttpRequestCocoaTask*)taskValue.pointerValue;
    if (remove)
        objc_removeAssociatedObjects(object);
    return task;
}

#pragma mark - NSURLSessionTaskDelegate

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(nullable NSError *)error
{
    NSLog(@"Session %@ download task %@ finished downloading with error %@\n",session, task, error);
    
    
    oxygine::HttpRequestCocoaTask* httpRequestTask = [self getTask:task remove:true];
    
    if (error)
    {
        httpRequestTask->complete_(true);
    }
    else
    {
        httpRequestTask->complete_(false);
    }
}


#pragma mark - NSURLSessionDataDelegate

// Not used yet (using completion handler for data tasks)
- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask  didReceiveData:(NSData *)data
{
    oxygine::HttpRequestCocoaTask* task = [self getTask:dataTask remove:false];
    if (!task)
        return;
    task->write(data);
}

- (void)URLSession:(NSURLSession *)session
          dataTask:(NSURLSessionDataTask *)dataTask
didReceiveResponse:(NSURLResponse *)response
 completionHandler:(void (^)(NSURLSessionResponseDisposition disposition))completionHandler
{
    if ([response isKindOfClass:[NSHTTPURLResponse class]])
    {
        NSHTTPURLResponse *httpResponse = ((NSHTTPURLResponse *)response);
        
        long long size = [httpResponse expectedContentLength];
        
        int resp = (int)httpResponse.statusCode;
        
        oxygine::HttpRequestCocoaTask* task = [self getTask:dataTask remove:false];
        if (task)
        {
            task->gotResponse(resp, size);
            bool ok = task->getResponseCodeChecker()(resp);
            completionHandler(ok ? NSURLSessionResponseAllow : NSURLSessionResponseCancel);
            return;
        }
    }

    
    completionHandler(NSURLSessionResponseAllow);
}

#pragma mark -

@end

namespace oxygine
{
    int _httpRequestTaskInitialized = 0;
    
    HttpRequests *_cls = nil;
    
    void _sessionConfigCommonInit(NSURLSessionConfiguration* config) {
        config.allowsCellularAccess = YES;
        config.timeoutIntervalForRequest = 30.0;
        config.timeoutIntervalForResource = 60.0;
        config.HTTPMaximumConnectionsPerHost = 10;
    }
    
    NSURLSession* __defaultSession = nil;
    NSURLSession* _getDefaultSession() {
        if (!__defaultSession) {
            OX_ASSERT(_cls); // Call HttpRequestTask::init()
            NSURLSessionConfiguration* config = [NSURLSessionConfiguration defaultSessionConfiguration];
            _sessionConfigCommonInit(config);
            __defaultSession = [NSURLSession sessionWithConfiguration:config delegate:_cls delegateQueue:nil];
        }
        return __defaultSession;
    }
    
    NSURLSession* __ephemeralSession = nil;
    NSURLSession* _getEphemeralSession() {
        if (!__ephemeralSession) {
            OX_ASSERT(_cls); // Call HttpRequestTask::init()
            NSURLSessionConfiguration* config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
            _sessionConfigCommonInit(config);
            config.URLCache = nil;
            config.URLCredentialStorage = nil;
            config.HTTPCookieStorage = nil;
            
            __ephemeralSession = [NSURLSession sessionWithConfiguration:config delegate:_cls delegateQueue:nil];
        }
        return __ephemeralSession;
    }
    
    void HttpRequestTask::init()
    {
        if (_httpRequestTaskInitialized++)
            return;
        
        setCustomRequests(createTask);
        _cls = [[HttpRequests alloc] init];
    }
    
    void HttpRequestTask::release()
    {
        OX_ASSERT(_httpRequestTaskInitialized > 0);
        if (--_httpRequestTaskInitialized)
            return;
        
        __defaultSession = nil;
        __ephemeralSession = nil;
        _cls = nil;
    }
    
    HttpRequestCocoaTask::HttpRequestCocoaTask()
    {
        
    }
    
    HttpRequestCocoaTask::~HttpRequestCocoaTask()
    {
        
    }
    
    void HttpRequestCocoaTask::progress_(int loaded, int total)
    {
        progress(loaded, total);
    }
    
    void HttpRequestCocoaTask::write(NSData *data)
    {
        const void *ptr = [data bytes];
        unsigned int len = [data length];
        HttpRequestTask::write(ptr, len);
    }
    
    void HttpRequestCocoaTask::complete_(bool error)
    {
        if (error)
            onError();
        else
            onComplete();
        
        releaseRef();
    }
    
    void HttpRequestCocoaTask::gotResponse(int resp, size_t expectedSize)
    {
        _responseCode = resp;
        
        _expectedContentSize = _receivedContentSize + expectedSize;
        
        progress(_receivedContentSize, _expectedContentSize);
        
    }
    
    void HttpRequestCocoaTask::_run()
    {
        addRef();
        
        NSString *urlString = [NSString stringWithUTF8String:_url.c_str()];
        NSURL *url = [NSURL URLWithString:urlString];
        
        NSURLSession *session = _cacheEnabled ? _getDefaultSession() : _getEphemeralSession();
        
        NSURLSessionTask *task = 0;
        
        NSMutableURLRequest *request = [NSMutableURLRequest	requestWithURL:url];
        for (const auto& h:_headers)
        {
            NSString *key = [NSString stringWithUTF8String:h.first.c_str()];
            NSString *value = [NSString stringWithUTF8String:h.second.c_str()];
            
            [request setValue:value forHTTPHeaderField:key];
        }
        
        if (!_postData.empty())
        {
            request.HTTPBody = [NSData dataWithBytes:_postData.data() length:_postData.size()];
            request.HTTPMethod = @"POST";
        }
            
        task = [session dataTaskWithRequest:request];
        
        NSValue *taskValue = [NSValue valueWithPointer:this];
        objc_setAssociatedObject(task, &taskKey, taskValue, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        
        log::messageln("created task session: %x", task);
        
        [task resume];
    }
}
