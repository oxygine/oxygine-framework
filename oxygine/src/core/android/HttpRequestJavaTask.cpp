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

        _jCreateRequestMethod = env->GetStaticMethodID(_jHttpRequestsClass, "createRequest", "(Ljava/lang/String;Ljava/lang/String;[BJ)Lorg/oxygine/lib/HttpRequestHolder;");
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
        env->CallStaticObjectMethod(_jHttpRequestsClass, jRelease);
    }

    spHttpRequestTask HttpRequestTask::create()
    {
        return new HttpRequestJavaTask();
    }

    HttpRequestJavaTask::HttpRequestJavaTask(): _handle(0)
    {

    }

    HttpRequestJavaTask::~HttpRequestJavaTask()
    {
        log::messageln("~HttpRequestJavaTask");
    }

    void HttpRequestJavaTask::_run()
    {
        addRef();
        _mainThreadSync = true;
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jstring jurl = jniGetEnv()->NewStringUTF(_url.c_str());
        jstring jfname = _fname.empty() ? 0 : jniGetEnv()->NewStringUTF(_fname.c_str());
        jbyteArray  jpost = 0;
        if (!_postData.empty())
        {
            jpost = env->NewByteArray(_postData.size());
            env->SetByteArrayRegion(jpost, 0, _postData.size(), (jbyte*)&_postData.front());
        }

        _handle = env->NewGlobalRef(env->CallStaticObjectMethod(_jHttpRequestsClass, _jCreateRequestMethod, jurl, jfname, jpost, (jlong)this));
    }

    void HttpRequestJavaTask::error_()
    {
        onError();
        releaseRef();
    }

    void HttpRequestJavaTask::progress_(int loaded, int total)
    {
        progress(loaded, total);
    }

    void HttpRequestJavaTask::complete_(jbyteArray ar)
    {
        if (ar)
        {
            JNIEnv* env = jniGetEnv();
            jbyte* data = env->GetByteArrayElements(ar, 0);
            jint len = env->GetArrayLength(ar);
            _response.assign((char*)data, (char*)data + len);
            env->ReleaseByteArrayElements(ar, data, 0);
        }

        onComplete();
        releaseRef();
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
    JNIEnv* Android_JNI_GetEnv(void);

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestResponseSuccess(JNIEnv* env, jlong handle, jbyteArray array)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->complete_(array);
    }

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestResponseProgress(JNIEnv* env, jlong handle, jint loaded, jint total)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->progress_(loaded, total);
    }

    JNIEXPORT void JNICALL Java_org_oxygine_lib_HttpRequest_nativeHttpRequestResponseError(JNIEnv* env, jlong handle)
    {
        oxygine::HttpRequestJavaTask* task = (oxygine::HttpRequestJavaTask*)handle;
        task->error_();
    }
}