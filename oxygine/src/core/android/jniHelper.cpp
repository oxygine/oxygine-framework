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

    void jniGetStringArray(vector<std::string>& res, JNIEnv* env, jobjectArray jarray)
    {
        jsize sz = env->GetArrayLength(jarray);
        res.reserve(sz);

        for (size_t i = 0; i < sz; ++i)
        {
            jstring obj = (jstring)env->GetObjectArrayElement(jarray, i);
            const char* str = env->GetStringUTFChars(obj, 0);

            res.emplace_back(str);

            env->ReleaseStringUTFChars(obj, str);
        }
    }
}