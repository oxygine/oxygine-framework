#pragma once
#include <jni.h> 
#include <android/log.h>
#include <string>
namespace oxygine
{
	using namespace std;
	
	#define STRINGIZE_DETAIL(x) #x
	#define STRINGIZE(x) STRINGIZE_DETAIL(x)

	template <bool D>
	class LocalReferenceHolder
	{
	public:
		LocalReferenceHolder(JNIEnv *env, const char *func, const char *line):_env(env), _func(func), _line(line)
		{
			if (D)
		 		__android_log_print(ANDROID_LOG_DEBUG, "SDL", "LocalReferenceHolder enter: %s %s", _func, _line);
			_env->PushLocalFrame(16);			
		}


		~LocalReferenceHolder()
		{
			_env->PopLocalFrame(0);

			if (D)
				__android_log_print(ANDROID_LOG_DEBUG, "SDL", "LocalReferenceHolder left: %s %s", _func, _line);
		}

		JNIEnv *_env;
		const char *_func;
		const char *_line;
	};

	#define LOCAL_REF_HOLDER(env) LocalReferenceHolder<false> _lr_(env, __func__, STRINGIZE(__LINE__));
	#define LOCAL_REF_HOLDER_D(env) LocalReferenceHolder<true> _lr_(env, __func__, STRINGIZE(__LINE__));

	class notFound {};

	#define JNI_NOT_NULL(v) if (!v) { __android_log_print(ANDROID_LOG_DEBUG, "SDL", "NOTNULL! %d %s", __LINE__, __FILE__); throw notFound();}


	JNIEnv* jniGetEnv(void);
	std::string jniGetString(JNIEnv *env, jstring jstr);
}