#pragma once
#include "../../oxygine-include.h"
#include "jniHelper.h"
#include "../oxygine.h"
#include <string>

namespace oxygine
{
    jobject         jniGetMainActivity();
    jclass          jniGetMainActivityClass();
    void            jniSetMainActivityClass(jclass);

    /**returns UTC time in MS*/
    int64           jniGetTimeUTCMS();

    /**returns locale. ISO 639-1 */
    std::string     jniGetLanguage();
    std::string     jniGetPackage();
    bool            jniIsNetworkAvailable();

    bool            jniExit();
    void            jniRestartApp();
    void            jniMoveTaskToBack();
    void            jniBrowse(const char* url);

    void            jniWriteBuffer2InternalStorageFile(const char* path, const char* data, size_t size);

    jobject         jniFindExtension(JNIEnv* env, jclass cl);


    std::string     jniGetProperty(const std::string& id);
}