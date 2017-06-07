#include "HttpRequestJavaTask.h"
#include "core/log.h"

namespace oxygine
{
    jclass _jHttpRequestsClass = 0;
    jmethodID _jCreateRequestMethod = 0;
    static HttpRequestTask* createTask()
    {
        return new HttpRequestJavaTask;
    }

    void HttpRequestTask::init()
    {
        setCustomRequests(createTask);

        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        _jHttpRequestsClass = (jclass) env->NewGlobalRef(env->FindClass("org/oxygine/lib/HttpRequests"));
        JNI_NOT_NULL(_jHttpRequestsClass);

        _jCreateRequestMethod = env->GetStaticMethodID(_jHttpRequestsClass, "createRequest",
                                "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;[BJ)Lorg/oxygine/lib/HttpRequestHolder;");

        JNI_NOT_NULL(_jCreateRequestMethod);

        jmethodID jInit = env->GetStaticMethodID(_jHttpRequestsClass, "init", "()V");
        JNI_NOT_NULL(jInit);
        env->CallStaticVoidMethod(_jHttpRequestsClass, jInit);

        log::messageln("initialized HttpRequestJavaTask");
    }

    void HttpRequestTask::release()
    {
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jmethodID jRelease = env->GetStaticMethodID(_jHttpRequestsClass, "release", "()V");
        JNI_NOT_NULL(jRelease);
        env->CallStaticVoidMethod(_jHttpRequestsClass, jRelease);

        env->DeleteGlobalRef(_jHttpRequestsClass);
        _jHttpRequestsClass = 0;
        _jCreateRequestMethod = 0;
    }

    HttpRequestJavaTask::HttpRequestJavaTask(): _handle(0)
    {
    }

    HttpRequestJavaTask::~HttpRequestJavaTask()
    {
    }

    void HttpRequestJavaTask::_run()
    {
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jstring jurl = env->NewStringUTF(_url.c_str());

        jbyteArray  jpost = 0;
        if (!_postData.empty())
        {
            jpost = env->NewByteArray(_postData.size());
            env->SetByteArrayRegion(jpost, 0, _postData.size(), (jbyte*)&_postData.front());
        }

        jobjectArray jkeys   = (jobjectArray)env->NewObjectArray(_headers.size(), env->FindClass("java/lang/String"), 0);
        jobjectArray jvalues = (jobjectArray)env->NewObjectArray(_headers.size(), env->FindClass("java/lang/String"), 0);

        for (size_t i = 0; i < _headers.size(); ++i)
        {
            jstring jkey   = env->NewStringUTF(_headers[i].first.c_str());
            jstring jvalue = env->NewStringUTF(_headers[i].second.c_str());

            env->SetObjectArrayElement(jkeys,   i, jkey);
            env->SetObjectArrayElement(jvalues, i, jvalue);
        }


        addRef();
        _handle = env->NewGlobalRef(env->CallStaticObjectMethod(_jHttpRequestsClass, _jCreateRequestMethod,
                                    jurl, jkeys, jvalues, jpost, (jlong)this));
    }

    void HttpRequestJavaTask::error_()
    {
        core::getMainThreadDispatcher().postCallback([ = ]() {
            _error();
            releaseRef();
        });
    }

    void HttpRequestJavaTask::gotHeader_(int respCode, int contentLen)
    {
        _responseCode = respCode;
        _expectedContentSize = contentLen;
        gotHeaders();
    }

    void HttpRequestJavaTask::write_(jbyteArray jdata, int size)
    {
        JNIEnv* env = jniGetEnv();
        jbyte* data = env->GetByteArrayElements(jdata, 0);

        write(data, size);

        env->ReleaseByteArrayElements(jdata, data, JNI_ABORT);
    }

    void HttpRequestJavaTask::complete_()
    {
        core::getMainThreadDispatcher().postCallback([ = ]() {
            _complete();
            releaseRef();
        });
    }

    void HttpRequestJavaTask::_finaliaze(bool)
    {
        JNIEnv* env = jniGetEnv();
        env->DeleteGlobalRef(_handle);
        _handle = 0;
    }
}

extern "C"
{
    /*
    public static native void nativeHttpRequestSuccess(long handle);
    public static native void nativeHttpRequestError(long handle);
    public static native void nativeHttpRequestGotHeader(long handle, int code, int contentLength);
    public static native void nativeHttpRequestWrite(long handle, byte[] data, int size);
    */
    JNIEnv* Android_JNI_GetEnv(void);

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestSuccess(JNIEnv* env, jclass, jlong handle)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->complete_();
    }

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestError(JNIEnv* env, jclass, jlong handle)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->error_();
    }

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestWrite(JNIEnv* env, jclass,
            jlong handle, jbyteArray array, jint size)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->write_(array, size);
    }

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestGotHeader(JNIEnv* env, jclass,
            jlong handle, jint respCode, jint contentLen)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->gotHeader_(respCode, contentLen);
    }
}