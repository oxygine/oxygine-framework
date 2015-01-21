#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#include "HttpRequestCocoaTask.h"
#include "file.h"
#include "STDFileSystem.h"
#include "core/log.h"
using namespace oxygine;
static char key;

@interface ObjectHolder:NSObject
{
    void* object;
}

@property(nonatomic, readwrite) void* object; // Property
@end
@implementation ObjectHolder
@synthesize object;
-(instancetype)initWithObject: (void*) obj
{
    object = obj;
    return self;
}
@end

@interface HttpRequests:NSObject<NSURLSessionDownloadDelegate, NSURLSessionDataDelegate>
{
	NSURLSession *_session;
}
@end

@implementation HttpRequests
-(id)init {
    self = [super init];
    
    NSURLSessionConfiguration* sessionConfig = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    
    sessionConfig.allowsCellularAccess = YES;
    sessionConfig.timeoutIntervalForRequest = 30.0;
    sessionConfig.timeoutIntervalForResource = 60.0;
    sessionConfig.HTTPMaximumConnectionsPerHost = 10;
    sessionConfig.URLCache = nil;
    sessionConfig.URLCredentialStorage = nil;
    sessionConfig.HTTPCookieStorage = nil;

    
    _session = [NSURLSession sessionWithConfiguration:sessionConfig delegate:self delegateQueue:[[NSOperationQueue alloc] init]];
    
    return self;
}

-(oxygine::HttpRequestCocoaTask*) getTask:(NSObject*)object remove:(BOOL)remove
{
    ObjectHolder *holder = objc_getAssociatedObject(object, &key);
    oxygine::HttpRequestCocoaTask* task = (oxygine::HttpRequestCocoaTask*)([holder object]);
    if (remove)
        objc_removeAssociatedObjects(object);
    if (!task)
        int q=0;
    return task;
}

-(void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)totalBytesWritten totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
    oxygine::HttpRequestCocoaTask* task = [self getTask:downloadTask remove:false];
    task->progress_(totalBytesWritten, totalBytesExpectedToWrite);
}

-(void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location {
    oxygine::HttpRequestCocoaTask* task = [self getTask:downloadTask remove:true];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];

    
    std::string dest = oxygine::file::wfs().getFullPath(task->getFileName().c_str());
    NSURL *destUrl = [NSURL fileURLWithPath:[NSString stringWithUTF8String:dest.c_str()]];
    
    //NSURL *originalUrl = [NSURL URLWithString:[downloadTaskURL lastPathComponent]];
    //NSURL *destinationUrl = [documentsDirectory URLByAppendingPathComponent:[originalUrl lastPathComponent]];
    NSError *fileManagerError;
    
    [fileManager removeItemAtURL:destUrl error:NULL];
    //ключевая  строчка!
    [fileManager copyItemAtURL:location toURL:destUrl error:&fileManagerError];
    task->complete_(nil);
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask  didReceiveData:(NSData *)data{
    oxygine::HttpRequestCocoaTask* task = [self getTask:dataTask remove:true];
    log::messageln("nssessiond complete %x", dataTask);
    //if (!task)
    //    return;
    task->complete_(data);
}

@end

namespace oxygine
{
    NSURLSession* _session = 0;
    

    void HttpRequestTask::init()
    {
        NSURLSessionConfiguration* config = [NSURLSessionConfiguration defaultSessionConfiguration];
        HttpRequests *cls = [[HttpRequests alloc] init];
        _session = [NSURLSession sessionWithConfiguration:config delegate:cls delegateQueue:[[NSOperationQueue alloc] init]];
    }
    
    void HttpRequestTask::release()
    {
        _session = 0;
    }
    
    spHttpRequestTask HttpRequestTask::create()
    {
        return new HttpRequestCocoaTask();
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
    
    void HttpRequestCocoaTask::complete_(NSData *data)
    {
        NSUInteger statusCode = 200;//((NSHTTPURLResponse *)response).statusCode;
        bool error = false;
        
        if (error || statusCode != 200)
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
        NSURL *url =[NSURL URLWithString:[NSString stringWithUTF8String:(_url).c_str()]];
        //NSURL *url = [NSURL alloc];
        //[url initWithString:[NSString stringWithUTF8String:_url.c_str()]];
        
        NSURLSessionTask *task = 0;
        if (_fname.empty())
        {
            NSMutableURLRequest *request = [NSMutableURLRequest	requestWithURL:url];
            if (!_postData.empty())
            {
                request.HTTPBody = [NSData dataWithBytes:_postData.data() length:_postData.size()];
                request.HTTPMethod = @"POST";
            }
            
            task = [_session dataTaskWithRequest:request
                completionHandler:^(NSData *data,
                                    NSURLResponse *response,
                                    NSError *error) {
                    // handle response
                    
                    NSUInteger statusCode = ((NSHTTPURLResponse *)response).statusCode;
                    

                    if (error || statusCode != 200)
                    {
                        onError();
                        releaseRef();
                    }
                    else
                    {
                        complete_(data);
                    }
                }];
        }
        else
        {
            task = [_session downloadTaskWithURL:url];

            /*
                               completionHandler:^(NSURL *location, NSURLResponse *response, NSError *error){
                                   // handle response
                                   NSUInteger statusCode = ((NSHTTPURLResponse *)response).statusCode;
                                   if (error || statusCode != 200)
                                       onError();
                                   else
                                   {
                                       onComplete();
                                   }
                                   releaseRef();
                               }];
             */
        }
        objc_setAssociatedObject(task, &key, [[ObjectHolder alloc] initWithObject:this], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        log::messageln("created task session: %x", task);
        
        [task resume];
    }
}