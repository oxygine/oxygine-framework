#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#include "HttpRequestCocoaTask.h"
#include "file.h"
#include "STDFileSystem.h"
#include "../log.h"
using namespace oxygine;
static char taskKey;


static HttpRequestTask *createTask()
{
    return new HttpRequestCocoaTask;
}

@interface HttpRequests:NSObject<NSURLSessionDownloadDelegate, NSURLSessionTaskDelegate, NSURLSessionDataDelegate>
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
    if (!task)
        int q=0;
    return task;
}

#pragma mark - NSURLSessionDownloadDelegate

-(void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)totalBytesWritten totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
    oxygine::HttpRequestCocoaTask* task = [self getTask:downloadTask remove:false];
    task->progress_((int) totalBytesWritten, (int) totalBytesExpectedToWrite);
}

-(void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location {
    
    oxygine::HttpRequestCocoaTask* task = [self getTask:downloadTask remove:true];
    
    NSHTTPURLResponse *resp = (NSHTTPURLResponse*)[downloadTask response];
    
    long code = [resp statusCode];
    
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    std::string dest = oxygine::file::wfs().getFullPath(task->getFileName().c_str());
    NSURL *destUrl = [NSURL fileURLWithPath:[NSString stringWithUTF8String:dest.c_str()]];
    
    NSError *fileManagerError;
    
    [fileManager removeItemAtURL:destUrl error:&fileManagerError];
    
    /*
    if (code == 200)
    {
        [fileManager copyItemAtURL:location toURL:destUrl error:&fileManagerError];
    
        task->complete_(nil,  false);
    }
    else
    {
        task->complete_(nil, true);
        
    }
    */

    task->complete_(nil, false, (int)code);
}

#pragma mark - NSURLSessionTaskDelegate

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(nullable NSError *)error
{
    NSLog(@"Session %@ download task %@ finished downloading with error %@\n",session, task, error);
    
    
    oxygine::HttpRequestCocoaTask* httpRequestTask = [self getTask:task remove:true];
    
    if (error) {
        NSData* resumeData = [error.userInfo objectForKey:NSURLSessionDownloadTaskResumeData];
        if (resumeData) {
            NSURLSessionTask *rt = [session downloadTaskWithResumeData:resumeData];
            [rt resume];
            
            NSValue *taskValue = [NSValue valueWithPointer:httpRequestTask];
            objc_setAssociatedObject(rt, &taskKey, taskValue, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        }
        else
        {
            httpRequestTask->complete_(/* data */ nil, /* error */ true, 0);
        }
    }
    
    if (!error) {
        // didFinishDownloadingToURL will be called in this case,
        // which will save the file and signal completion.
    } else {
        
    }
}


#pragma mark - NSURLSessionDataDelegate

// Not used yet (using completion handler for data tasks)
//- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask  didReceiveData:(NSData *)data{
//    oxygine::HttpRequestCocoaTask* task = [self getTask:dataTask remove:true];
//    log::messageln("nssessiond complete %x", dataTask);
//    //if (!task)
//    //    return;
//    task->complete_(data);
//}

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
    
    void HttpRequestCocoaTask::complete_(NSData *data, bool error, int respCode)
    {
        _responseCode = respCode;
        
        if (error)
            onError();
        else
        {
            if (data)
            {
                const void *ptr = [data bytes];
                size_t len = [data length];
                _response.assign((const char*)ptr, (const char*)ptr + len);
            }
            
            onComplete();
        }
        
        releaseRef();
    }
    
    void HttpRequestCocoaTask::_run()
    {
        _mainThreadSync = true;
        
        addRef();
        NSString *urlString = [NSString stringWithUTF8String:_url.c_str()];
        NSURL *url =[NSURL URLWithString:urlString];
        
        NSURLSession *session = _cacheEnabled ? _getDefaultSession() : _getEphemeralSession();
        
        NSURLSessionTask *task = 0;
        if (_fname.empty())
        {
            NSMutableURLRequest *request = [NSMutableURLRequest	requestWithURL:url];
            if (!_postData.empty())
            {
                request.HTTPBody = [NSData dataWithBytes:_postData.data() length:_postData.size()];
                request.HTTPMethod = @"POST";
            }
            
            task = [session dataTaskWithRequest:request
                              completionHandler:^(NSData *data,
                                                  NSURLResponse *response,
                                                  NSError *error) {
                                  // handle response
                                  
                                  
                                  if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                                      NSHTTPURLResponse *httpResponse = ((NSHTTPURLResponse *)response);
                                      _responseCode = (int)httpResponse.statusCode;
                                      //if (statusCode != 200)
                                       //   httpError = true;
                                  }
                                  
                                  if (error)
                                  {
                                      onError();
                                  }
                                  else
                                  {
                                      if (data)
                                      {
                                          const void *ptr = data.bytes;
                                          size_t len = data.length;
                                          _response.assign((const char*)ptr, (const char*)ptr + len);
                                      }
                                      
                                      onComplete();
                                  }
                                  releaseRef();
                              }];
        }
        else
        {
            task = [session downloadTaskWithURL:url];
        }
        
        NSValue *taskValue = [NSValue valueWithPointer:this];
        objc_setAssociatedObject(task, &taskKey, taskValue, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        
        log::messageln("created task session: %x", task);
        
        [task resume];
    }
}
