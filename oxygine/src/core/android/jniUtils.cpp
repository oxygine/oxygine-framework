#include "jniUtils.h"
#include "jniHelper.h"
#include "core/log.h"

jobject _jmainActivity = 0;
jclass _jmainActivityClass = 0;
jclass _jUtils = 0;

jmethodID _jUtils_getTimeUTCMS = 0;
jmethodID _jUtils_getLanguage = 0;
jmethodID _jUtils_isNetworkAvailable = 0;

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

            _jUtils_isNetworkAvailable = env->GetStaticMethodID(_jUtils, "isNetworkAvailable", "()Z");
            JNI_NOT_NULL(_jUtils_isNetworkAvailable);
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
        try
        {
            JNIEnv* env = jniGetEnv();
            LOCAL_REF_HOLDER(env);

            jstring jstr = (jstring)env->CallStaticObjectMethod(_jUtils, _jUtils_getLanguage);

            return jniGetString(env, jstr);
        }
        catch (const notFound&)
        {

        }

        return "";
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
}

static void _init(JNIEnv* env)
{
    oxygine::_jniInit(env);
}

extern "C"
{
    /*
     * Class:     org_oxygine_OxygineActivity
     * Method:    nativeOxygineInit
     * Signature: (Lorg/oxygine/OxygineActivity;)V
     */


    JNIEXPORT void JNICALL Java_org_oxygine_lib_OxygineActivity_nativeOxygineInit
    (JNIEnv* env, jclass clazz, jobject object)
    {
        _jmainActivityClass = (jclass)env->NewGlobalRef(clazz);
        _jmainActivity = env->NewGlobalRef(object);

        _init(env);
    }
}