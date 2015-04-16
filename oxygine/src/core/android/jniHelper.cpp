#include "jniHelper.h"

extern "C"
{
    JNIEnv* Android_JNI_GetEnv(void);
}

namespace oxygine
{
    JNIEnv* jniGetEnv(void)
    {
        return Android_JNI_GetEnv();
    }

    std::string jniGetString(JNIEnv* env, jstring jstr)
    {
        if (!jstr)
            return "";
        const char* cstr = env->GetStringUTFChars(jstr, 0);
        string str = cstr;
        env->ReleaseStringUTFChars(jstr, cstr);
        return str;
    }
}