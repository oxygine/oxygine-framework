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
        if (jarray == 0)
            return;

        jsize sz = env->GetArrayLength(jarray);
        res.reserve(sz);

        for (size_t i = 0; i < sz; ++i)
        {
            jstring jstr = (jstring)env->GetObjectArrayElement(jarray, i);
            std::string str = jniGetString(env, jstr);
            res.emplace_back(str);
        }
    }
}