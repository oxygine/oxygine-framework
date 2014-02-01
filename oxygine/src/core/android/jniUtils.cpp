#include "jniUtils.h"
#include "jnihelper.h"

jobject _jmainActivity = 0;
jclass _jmainActivityClass = 0;
jclass _jUtils = 0;

jmethodID _jUtils_getTimeUTCMS = 0;
jmethodID _jUtils_getLanguage = 0;
jmethodID _jUtils_isNetworkAvailable = 0;

namespace oxygine
{
	using namespace std;
	
	jobject			jniGetMainActivity()
	{
		return _jmainActivity;
	}

	jclass			jniGetMainActivityClass()
	{
		return _jmainActivityClass;
	}

	void			jniSetMainActivityClass(jclass c)
	{		
		_jmainActivityClass = c;
	}

	void _jniInit(JNIEnv *env)
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
		catch(const notFound&)
		{

		}

	}

	int64 jniGetTimeUTCMS()
	{
		try
		{
			JNIEnv *env = jniGetEnv();
			LOCAL_REF_HOLDER(env);

			jlong value = env->CallStaticLongMethod(_jUtils, _jUtils_getTimeUTCMS);
			return value;
		}
		catch(const notFound&)
		{

		}

		return 0;
	}

	std::string 	getString(JNIEnv *env, jstring jstr)
	{
		const char *cstr = env->GetStringUTFChars(jstr, 0);
		string str = cstr;
		env->ReleaseStringUTFChars(jstr, cstr);
		return str;
	}

	std::string		jniGetLanguage()
	{
		try
		{
			JNIEnv *env = jniGetEnv();
			LOCAL_REF_HOLDER(env);

			jstring jstr = (jstring)env->CallStaticObjectMethod(_jUtils, _jUtils_getLanguage);

			return getString(env, jstr);
		}
		catch(const notFound&)
		{

		}

		return "";
	}

	bool			jniIsNetworkAvailable()
	{
		try
		{
			JNIEnv *env = jniGetEnv();
			LOCAL_REF_HOLDER(env);

			jboolean value = env->CallStaticBooleanMethod(_jUtils, _jUtils_isNetworkAvailable);
			return value;
		}
		catch(const notFound&)
		{

		}

		return false;
	}
}

static void _init(JNIEnv *env)
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
		(JNIEnv *env, jclass clazz, jobject object)
	{
		_jmainActivityClass = (jclass)env->NewGlobalRef(clazz);
		_jmainActivity = env->NewGlobalRef(object);

		_init(env);
	}
}