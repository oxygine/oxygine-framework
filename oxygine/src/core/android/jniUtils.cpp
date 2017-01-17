#include "jniUtils.h"
#include "jniHelper.h"
#include "core/log.h"
#include "core/ThreadDispatcher.h"

using namespace oxygine;

jobject _jmainActivity = 0;
jclass _jmainActivityClass = 0;
jclass _jUtils = 0;

jmethodID _jUtils_getTimeUTCMS = 0;
jmethodID _jUtils_getLanguage = 0;
jmethodID _jUtils_getPackage = 0;
jmethodID _jUtils_getProperty = 0;
jmethodID _jUtils_isNetworkAvailable = 0;
jmethodID _jRunnable_run = 0;

namespace oxygine
{
    using namespace std;

    jobject         jniGetMainActivity()
    {
        return _jmainActivity;
    }

    jclass          jniGetMainActivityClass()
    {
        return _jmainActivityClass;
    }

    void            jniSetMainActivityClass(jclass c)
    {
        _jmainActivityClass = c;
    }

    void _jniInit(JNIEnv* env)
    {
        try
        {
            LOCAL_REF_HOLDER(env);

            _jUtils = (jclass)env->NewGlobalRef(env->FindClass("org/oxygine/lib/Utils"));
            JNI_NOT_NULL(_jUtils);

            _jUtils_getTimeUTCMS = env->GetStaticMethodID(_jUtils, "getTimeUTCMS", "()J");
            JNI_NOT_NULL(_jUtils_getTimeUTCMS);

            _jUtils_getLanguage = env->GetStaticMethodID(_jUtils, "getLanguage", "()Ljava/lang/String;");
            JNI_NOT_NULL(_jUtils_getLanguage);

            _jUtils_getPackage = env->GetStaticMethodID(_jUtils, "getPackage", "()Ljava/lang/String;");
            JNI_NOT_NULL(_jUtils_getPackage);

            _jUtils_getProperty = env->GetStaticMethodID(_jUtils, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
            JNI_NOT_NULL(_jUtils_getProperty);

            _jUtils_isNetworkAvailable = env->GetStaticMethodID(_jUtils, "isNetworkAvailable", "()Z");
            JNI_NOT_NULL(_jUtils_isNetworkAvailable);


            jclass rn = env->FindClass("java/lang/Runnable");
            JNI_NOT_NULL(rn);
            _jRunnable_run = env->GetMethodID(rn, "run", "()V");
            JNI_NOT_NULL(_jRunnable_run);
        }
        catch (const notFound&)
        {

        }

    }

    int64 jniGetTimeUTCMS()
    {
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jlong value = env->CallStaticLongMethod(_jUtils, _jUtils_getTimeUTCMS);
            return value;
        }
        catch (const notFound&)
        {

        }

        return 0;
    }

    std::string     jniGetLanguage()
    {
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jstring jstr = (jstring)env->CallStaticObjectMethod(_jUtils, _jUtils_getLanguage);

        return jniGetString(env, jstr);
    }

    std::string jniGetPackage()
    {
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jstring jstr = (jstring)env->CallStaticObjectMethod(_jUtils, _jUtils_getPackage);

        return jniGetString(env, jstr);
    }

    bool            jniIsNetworkAvailable()
    {
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jboolean value = env->CallStaticBooleanMethod(_jUtils, _jUtils_isNetworkAvailable);
            return value;
        }
        catch (const notFound&)
        {

        }

        return false;
    }

    bool            jniExit()
    {
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jmethodID m = env->GetStaticMethodID(_jUtils, "exit", "()V");
            JNI_NOT_NULL(m);
            env->CallStaticVoidMethod(_jUtils, m);            
        }
        catch (const notFound&) {}

        return true;
    }

    void            jniRestartApp()
    {
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jmethodID m = env->GetStaticMethodID(_jUtils, "restartApp", "()V");
        JNI_NOT_NULL(m);
        env->CallStaticVoidMethod(_jUtils, m);
    }

    void            jniMoveTaskToBack()
    {
        log::messageln("jniMoveTaskToBack");
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jmethodID m = env->GetStaticMethodID(_jUtils, "moveTaskToBack", "()V");
            JNI_NOT_NULL(m);
            env->CallStaticVoidMethod(_jUtils, m);
        }
        catch (const notFound&) {}
    }

    void            jniBrowse(const char* url)
    {
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jstring jstr = env->NewStringUTF(url);

            jmethodID m = env->GetStaticMethodID(_jUtils, "browse", "(Ljava/lang/String;)V");
            JNI_NOT_NULL(m);
            env->CallStaticVoidMethod(_jUtils, m, jstr);
        }
        catch (const notFound&) {}
    }

    void            jniWriteBuffer2InternalStorageFile(const char* path, const char* data, size_t size)
    {
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jmethodID m = env->GetStaticMethodID(_jUtils, "writeBuffer2InternalStorage", "(Ljava/lang/String;[B)V");
            jbyteArray buffer = env->NewByteArray(size);
            env->SetByteArrayRegion(buffer, 0, size, (const jbyte*)data);
            JNI_NOT_NULL(m);
            env->CallStaticVoidMethod(_jUtils, m, env->NewStringUTF(path), buffer);
        }
        catch (const notFound&) {}
    }



    jobject jniFindExtension(JNIEnv* env, jclass cl)
    {
        jobject r;

        try
        {
            jmethodID m = env->GetMethodID(jniGetMainActivityClass(), "findClass", "(Ljava/lang/Class;)Lorg/oxygine/lib/extension/ActivityObserver;");
            JNI_NOT_NULL(m);

            r = env->CallObjectMethod(jniGetMainActivity(), m, cl);
            JNI_NOT_NULL(r);
        }
        catch (const notFound&)
        {

        }

        return r;
    }

    std::string     jniGetProperty(const std::string& id)
    {
        JNIEnv* env = jniGetEnv();
        LOCAL_REF_HOLDER(env);

        jstring jarg = env->NewStringUTF(id.c_str());
        jstring jstr = (jstring)env->CallStaticObjectMethod(_jUtils, _jUtils_getProperty, jarg);


        return jniGetString(env, jstr);
    }
}

static void _init(JNIEnv* env)
{
    oxygine::_jniInit(env);
}


void runTask(const ThreadMessages::message& m)
{
    JNIEnv* env = jniGetEnv();

    jobject task = (jobject)m.arg1;
    env->CallVoidMethod(task, _jRunnable_run);
    env->DeleteGlobalRef(task);
}

extern "C"
{
    /*
     * Class:     org_oxygine_OxygineActivity
     * Method:    nativeOxygineInit
     * Signature: (Lorg/oxygine/OxygineActivity;)V
     */


    JNIEXPORT void JNICALL Java_org_oxygine_lib_OxygineActivity_nativeOxygineInit
    (JNIEnv* env, jclass clazz, jobject act, jclass actClass)
    {
        _jmainActivityClass = (jclass)env->NewGlobalRef(actClass);
        _jmainActivity = env->NewGlobalRef(act);

        _init(env);
    }

    JNIEXPORT void JNICALL Java_org_oxygine_lib_Utils_runOnGameThread
    (JNIEnv* env, jclass clazz, jobject object)
    {
        core::getMainThreadDispatcher().postCallback(0, env->NewGlobalRef(object), 0, runTask, 0);
    }
}