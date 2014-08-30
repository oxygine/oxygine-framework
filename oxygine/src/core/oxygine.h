#pragma once
#include "oxygine_include.h"
#include "intrusive_ptr.h"
#include "ref_counter.h"
#include "closure/closure.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix.h"
#include "log.h"
#include "ox_debug.h"
#include "ThreadMessages.h"
#include <string>

#ifndef __S3E__
#if defined(_MSC_VER) || defined(__BORLANDC__)
	typedef unsigned __int64 uint64;
	typedef signed __int64 int64;
#else
	typedef unsigned long long uint64;
	typedef signed long long int64;
#endif
#endif


//makefourcc('_', '_', '_', '_')
#define makefourcc(a,b,c,d) ( ((unsigned int)a) | (((unsigned int)b)<< 8) | (((unsigned int)c)<<16) | (((unsigned int)d)<<24))

/**main oxygine namespace*/
namespace oxygine
{

	void *fastAlloc(size_t size);
	void fastFree(void *data);

	typedef int timeMS;



	template <class dest, class src>
	dest safeCast(src ptr)
	{
		if (!ptr)
			return 0;
#ifdef OXYGINE_DEBUG_SAFECAST
		dest cast = dynamic_cast<dest>(ptr);
		OX_ASSERT(cast && "can't cast pointer");
		return cast;
#endif
		return static_cast<dest>(ptr);
	}

	template<class T, class U>
	intrusive_ptr<T> safeSpCast(intrusive_ptr<U> const & p)
	{
		if (!p)
			return 0;
#ifdef OXYGINE_DEBUG_SAFECAST
		intrusive_ptr<T> t = dynamic_cast<T *>(p.get());
		OX_ASSERT(t && "can't cast pointer");
#endif
		return static_cast<T *>(p.get());
	}

	/** returns local app time in milliseconds (1sec = 1000ms). Counting starts from zero*/	
	timeMS			getTimeMS();
	int64			getTimeUTCMS();
	bool			isNetworkAvaible();
	/**returns locale. ISO 639-1 */
	std::string		getLanguage();



	void	sleep(timeMS);
	
		
	namespace core
	{
		struct init_desc
		{
			init_desc(): mode24bpp(true), w(-1), h(-1), fullscreen(false), title("Oxygine"){}

			bool mode24bpp;
			int w;
			int h;
			bool fullscreen;
			const char *title;
		};

		/** Initializes engine internal components*/
		void init(init_desc *desc = 0);
		void init2();
		void release();
		void requestQuit();

		/**destroy and reset any GPU allocated memory and handles. Call it to free memory if app was minimized (lost focus)*/
		void reset();
		/**restores GPU memory state after reset*/
		void restore();

		/** Update engine*/
		bool update();
		/** Swap Video buffers*/
		void swapDisplayBuffers();
		void execute(const char *);

		/** Returns display size in pixels*/
		Point getDisplaySize();

		ThreadMessages& getMainThreadMessages(); 

#ifdef OXYGINE_SDL
		bool isActive();
		bool hasFocus();
#endif
	}
	


	enum error_policy
	{
		ep_show_error,//shows assert and prints error to log
		ep_show_warning,//prints warning to log
		ep_ignore_error//doesn't show any errors
	};


	void handleErrorPolicy(error_policy ep, const char *format, ...);
}
