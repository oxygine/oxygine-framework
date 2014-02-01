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
}