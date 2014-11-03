#include "oxygine.h"
#include <stdio.h>
#include "Renderer.h"
#include "log.h"
#include "winnie_alloc/winnie_alloc.h"
#include "VideoDriver.h"
#include "res/CreateResourceContext.h"
#include "res/Resources.h"

#include "res/ResBuffer.h"
#include "res/ResFontBM.h"
#include "res/ResAtlas.h"
#include "MemoryTexture.h"
#include "PointerState.h"
#include "Input.h"

#include "Renderer.h"
#include "DebugActor.h"
#include "Stage.h"

#include "res/ResStarlingAtlas.h"


#include "Mutex.h"

#include "gl/oxgl.h"
#include "winnie_alloc/winnie_alloc.h"
#include "ThreadMessages.h"

#ifdef __S3E__
#include "s3e.h"
#include "IwGL.h"
#include "s3eSurface.h"
#include "s3eDebug.h"
#include "s3eTimer.h"
#include "s3ePointer.h"
#include "s3eOSExec.h"
#endif


#include "gl/VideoDriverGLES20.h"


#ifdef EMSCRIPTEN
//#include <EGL/egl.h>
#include <sys/time.h>
#include <emscripten.h>
#include <SDL.h>
#include <SDL_compat.h>
#endif

#ifdef __ANDROID__
#include "core/android/jniUtils.h"
#endif

#ifdef OXYGINE_SDL
#include "coroutines.h"

extern "C"
{
	#include "SDL.h"
	#include "SDL_video.h"
};

#include <stdio.h>
#include "SDL_syswm.h"
#include "SDL_timer.h"

#endif


#if !SDL_VIDEO_OPENGL					
#define HANDLE_FOCUS_LOST 1
#else
#define HANDLE_FOCUS_LOST 0
#endif


namespace oxygine
{
	namespace file
	{
		void init();
	}



	IVideoDriver::Stats _videoStats;

	ThreadMessages _threadMessages;

    void checkGLError()
    {
        int gl_error = glGetError();
        if (gl_error != GL_NO_ERROR)
        {
            log::error("OpenGL error: %#x\n", gl_error);
            if (gl_error == GL_OUT_OF_MEMORY)
            {
                exit(0);
            }
        }
    }
    
	Mutex mutexAlloc;

	void *fastAlloc(size_t size)
	{		
#ifndef USE_MEMORY_POOL
		void *data = malloc(size);		
#else
		MutexAutoLock m(mutexAlloc);
		void *data = Winnie::Alloc(size);
#endif
		//memset(data, 0xcdcdcdcd, size);
		//memset(data, 0, size);

		return data;
	}

	void fastFree(void *data)
	{
#ifndef USE_MEMORY_POOL
		free(data);		
#else
		MutexAutoLock m(mutexAlloc);
		Winnie::Free(data);
#endif
	}


#if USE_EGL || EMSCRIPTEN
	//EGLDisplay			eglDisplay	= 0;
	//EGLSurface			eglSurface	= 0;

	Point _displaySize(0,0);
#endif


	timeMS getTimeMS()
	{
#if __S3E__
		return (timeMS)s3eTimerGetUST();
#elif OXYGINE_SDL
		return SDL_GetTicks();
#elif EMSCRIPTEN
		static bool init = false;
		static struct timespec start_ts;
		if (!init)
		{
			init = true;
			clock_gettime(CLOCK_MONOTONIC, &start_ts);
		}
		
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		timeMS ticks = (now.tv_sec - start_ts.tv_sec) * 1000 + (now.tv_nsec -
			start_ts.tv_nsec) / 1000000; 

		return ticks;
#else
		#pragma error "getTimeMS not implemented"
#endif		
		return 0;
	}
	
	core::init_desc desc;

	
#ifdef __S3E__

	int32 pointerEvent(void *sysData, void *u)
	{
		s3ePointerEvent *ev = (s3ePointerEvent*)sysData;
		int type = 0;
		switch(ev->m_Button)
		{
		case S3E_POINTER_BUTTON_SELECT:
			type = ev->m_Pressed ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP;
			break;
		case S3E_POINTER_BUTTON_MOUSEWHEELUP:
			type = TouchEvent::WHEEL_UP;
			break;
		case S3E_POINTER_BUTTON_MOUSEWHEELDOWN:
			type = TouchEvent::WHEEL_DOWN;
			break;
		}
				
		MouseButton b = MouseButton_Left;
		switch(ev->m_Button)
		{
		case S3E_POINTER_BUTTON_LEFTMOUSE: b = MouseButton_Left; break;
		case S3E_POINTER_BUTTON_RIGHTMOUSE: b = MouseButton_Right; break;
		case S3E_POINTER_BUTTON_MIDDLEMOUSE: b = MouseButton_Middle; break;
		}

		Input::instance.sendPointerButtonEvent(b, (float)ev->m_x, (float)ev->m_y, 1.0f, type, &Input::instance._pointerMouse);

		return 0;
	}

	int32 pointerMotionEvent(void *sysData, void *u)
	{
		s3ePointerMotionEvent *ev = (s3ePointerMotionEvent*)sysData;

		Input::instance.sendPointerMotionEvent((float)ev->m_x, (float)ev->m_y, 1.0f, &Input::instance._pointerMouse);
		return 0;
	}

	int32 pointerTouchEvent(void *sysData, void *u)
	{
		Input *This = (Input*)u;
		s3ePointerTouchEvent *ev = (s3ePointerTouchEvent*)sysData;
		int id = ev->m_TouchID + 1;

		Input::instance.sendPointerButtonEvent(MouseButton_Touch, (float)ev->m_x, (float)ev->m_y, 1.0f, ev->m_Pressed ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP, Input::instance.getTouchByIndex(id));

		return 0;
	}

	int32 pointerTouchMotionEvent(void *sysData, void *u)
	{
		Input *This = (Input*)u;
		s3ePointerTouchMotionEvent *ev = (s3ePointerTouchMotionEvent*)sysData;
		int id = ev->m_TouchID + 1;

		Input::instance.sendPointerMotionEvent((float)ev->m_x, (float)ev->m_y, 1.0f, Input::instance.getTouchByIndex(id));

		return 0;
	}

	int32 applicationPause(void* systemData, void* userData)
	{
		Event ev(Stage::DEACTIVATE);
		getStage()->dispatchEvent(&ev);
		return 0;
	}

	int32 applicationUnPause(void* systemData, void* userData)
	{
		Event ev(Stage::ACTIVATE);
		getStage()->dispatchEvent(&ev);
		return 0;
	}
#endif

#ifdef OXYGINE_SDL
	SDL_Window *_window = 0;
	SDL_GLContext _context = 0;
#endif

	namespace core
	{
		void focusLost()
		{			
			return;
#if OXYGINE_SDL
			log::messageln("focus lost");
			SDL_GL_DeleteContext(_context);
			_context = 0;
#endif
			
		}

		void lostContext()
		{
			return;
#if OXYGINE_SDL			
			log::messageln("lost context");			
			if(!_context)
			{
				_context = SDL_GL_CreateContext(_window);
				initGLExtensions(SDL_GL_GetProcAddress);
			}
#endif			
		}

		void init(init_desc *desc_ptr)
		{
			Input::instance.__removeFromDebugList();

			log::messageln("initialize oxygine");
			if (desc_ptr)
				desc = *desc_ptr;

	#ifdef __S3E__
            log::messageln("S3E build");
			if (!IwGLInit())
			{
				s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "IwGLInit failed");
				return;
			}
			//init_ext();

			int width = IwGLGetInt(IW_GL_WIDTH);
			int height = IwGLGetInt(IW_GL_HEIGHT);

			log::messageln("Screen BPP  : %d", s3eSurfaceGetInt(S3E_SURFACE_PIXEL_TYPE) & S3E_SURFACE_PIXEL_SIZE_MASK);
			log::messageln("Screen Size : %dx%d", width, height);
			log::messageln("Vendor      : %s", (const char*)glGetString( GL_VENDOR ) );
			log::messageln("Renderer    : %s", (const char*)glGetString( GL_RENDERER ));

			const char *version = (const char*)glGetString( GL_VERSION );
			log::messageln( "Version    : %s", version);

			s3ePointerUpdate();
			if (s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE))
			{
				s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, &pointerTouchEvent, 0);
				s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, &pointerTouchMotionEvent, 0);
			}
			else
			{
				s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, &pointerEvent, 0);
				s3ePointerRegister(S3E_POINTER_MOTION_EVENT, &pointerMotionEvent, 0);
			}	

			s3eDeviceRegister(S3E_DEVICE_UNPAUSE, applicationUnPause, 0);
			s3eDeviceRegister(S3E_DEVICE_PAUSE, applicationPause, 0);
	#endif
	
	#if __FLASHPLAYER__
	#endif

	#if EMSCRIPTEN
			log::messageln("EMSCRIPTEN build");

			if (desc.w == -1 && desc.h == -1)
			{
				int fs = 0;
				emscripten_get_canvas_size(&desc.w, &desc.h, &fs);
			}
			
			if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) 
			{
				log::error("Unable to initialize SDL: %s\n", SDL_GetError());				
			}

			SDL_Surface *screen;
			screen = SDL_SetVideoMode(desc.w, desc.h, 32, SDL_OPENGL); 
			_displaySize = Point(desc.w, desc.h);
			/*


			EGLConfig			eglConfig = 0;

			EGLContext			eglContext = 0;
			NativeWindowType	eglWindow = 0;
			EGLint				pi32ConfigAttribs[128];

#define CHECK_EGL_ERROR() {int v = eglGetError(); if (v != EGL_SUCCESS) log::error("egl error %d", v);}

			eglDisplay = eglGetDisplay((NativeDisplayType)0);

			CHECK_EGL_ERROR();

			

			CHECK_EGL_ERROR();

			EGLint iMajorVersion = 2 , iMinorVersion = 1;
			if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
			{
				log::error("eglInitialize failed");
			}
			
			CHECK_EGL_ERROR();
			
			bool mode24bpp = desc.mode24bpp;
			int i = 0;
			
			pi32ConfigAttribs[i++] = EGL_RED_SIZE;
			pi32ConfigAttribs[i++] = mode24bpp ? 8 : 5;
			pi32ConfigAttribs[i++] = EGL_GREEN_SIZE;
			pi32ConfigAttribs[i++] = mode24bpp ? 8 : 6;
			pi32ConfigAttribs[i++] = EGL_BLUE_SIZE;
			pi32ConfigAttribs[i++] = mode24bpp ? 8 : 5;
			pi32ConfigAttribs[i++] = EGL_ALPHA_SIZE;
			pi32ConfigAttribs[i++] = 0;
			pi32ConfigAttribs[i++] = EGL_SURFACE_TYPE;
			pi32ConfigAttribs[i++] = EGL_WINDOW_BIT;			
			pi32ConfigAttribs[i++] = EGL_NONE;			
			


			EGLint iConfigs;
			if (!eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
			{
				log::error("eglChooseConfig failed");
			}

			CHECK_EGL_ERROR();

			eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);

			if (eglSurface == EGL_NO_SURFACE)
			{
				eglGetError(); // Clear error
				eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, NULL, NULL);
			}

			CHECK_EGL_ERROR();
			int attrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
			eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, attrs);
			eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
			CHECK_EGL_ERROR();

			checkGLError();
			log::messageln("egl initialized");
			*/

	#endif
		
	#ifdef OXYGINE_SDL

			log::messageln("SDL build");
			int flags = SDL_WINDOW_SHOWN;

	#ifndef USE_EGL
			SDL_Init(SDL_INIT_VIDEO);


			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
			SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);			
			//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
            
#if TARGET_OS_IPHONE
            flags |= SDL_WINDOW_BORDERLESS;
#endif            
	#endif
            

			SDL_DisplayMode mode;
			SDL_GetCurrentDisplayMode(0, &mode);
			log::messageln("display mode: %d %d", mode.w, mode.h);

            if (desc.w == -1 && desc.h == -1)
            {
                desc.w = 960;
                desc.h = 640;
            }

			if (desc.fullscreen)
				flags |= SDL_WINDOW_FULLSCREEN;

            log::messageln("creating window %d %d", desc.w, desc.h);

			_window = SDL_CreateWindow(desc.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, desc.w, desc.h, flags);
	#ifndef USE_EGL
			if (!_window)
			{
				log::error("can't create window: %s", SDL_GetError());
                return;	
			}
			_context = SDL_GL_CreateContext(_window);
            if (!_context)
            {
                log::error("can't create gl context: %s", SDL_GetError());
                return;
            }

			SDL_GL_SetSwapInterval(desc.vsync ? 1 : 0);
			
	#else

			HWND				hWnd	= 0;
			HDC					hDC		= 0;

			EGLConfig			eglConfig	= 0;
		
			EGLContext			eglContext	= 0;
			NativeWindowType	eglWindow	= 0;
			EGLint				pi32ConfigAttribs[128];

	
			SDL_SysWMinfo wmi;
			SDL_VERSION(&wmi.version);
			SDL_GetWindowWMInfo(_window, &wmi);
	
			hWnd = (HWND)wmi.info.win.window;

			hDC = GetDC(hWnd);
			eglDisplay = eglGetDisplay((NativeDisplayType) hDC);
	

			eglWindow = hWnd;
			if(eglDisplay == EGL_NO_DISPLAY)
				 eglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);

			EGLint iMajorVersion, iMinorVersion;
			if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
			{
				int q=0;
			}

			bool mode24bpp = desc.mode24bpp;
			int i = 0;
			pi32ConfigAttribs[i++] = EGL_RED_SIZE;
			pi32ConfigAttribs[i++] = mode24bpp ? 8 : 5;
			pi32ConfigAttribs[i++] = EGL_GREEN_SIZE;
			pi32ConfigAttribs[i++] = mode24bpp ? 8 : 6;
			pi32ConfigAttribs[i++] = EGL_BLUE_SIZE;
			pi32ConfigAttribs[i++] = mode24bpp ? 8 : 5;
			pi32ConfigAttribs[i++] = EGL_ALPHA_SIZE;
			pi32ConfigAttribs[i++] = 0;
			pi32ConfigAttribs[i++] = EGL_SURFACE_TYPE;
			pi32ConfigAttribs[i++] = EGL_WINDOW_BIT;
			pi32ConfigAttribs[i++] = EGL_NONE;

			EGLint iConfigs;
			if (!eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
			{
				int q=0;
			}
			eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);

			if(eglSurface == EGL_NO_SURFACE)
			{
				eglGetError(); // Clear error
				eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, NULL, NULL);
			}
			eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
			eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	#endif

	#endif

            file::init();

			init2();			
		}

		void init2()
		{
#ifdef OXYGINE_SDL
			initGLExtensions(SDL_GL_GetProcAddress);
#endif

			Point size = getDisplaySize();
			log::messageln("display size: %d %d", size.x, size.y);


#if __S3E__
			int glversion = s3eGLGetInt(S3E_GL_VERSION);
			int major_gl = glversion >> 8;

			if (major_gl == 2)
				IVideoDriver::instance = new VideoDriverGLES20();
			else
			{
				OX_ASSERT(!"gl version should be 2");
				//IVideoDriver::instance = new VideoDriverGLES11();			
			}

#elif __FLASHPLAYER__
			{
				VideoDriverStage3D *vd = new VideoDriverStage3D();
				vd->init();
				IVideoDriver::instance = vd;
			}

			//IVideoDriver::instance = new VideoDriverNull();
#else
			IVideoDriver::instance = new VideoDriverGLES20();			
#endif


			checkGLError();

			IVideoDriver::instance->setDefaultSettings();

			checkGLError();

			Renderer::initialize();

			Resources::registerResourceType(ResAtlas::create, "atlas");
			Resources::registerResourceType(ResBuffer::create, "buffer");
			Resources::registerResourceType(ResFontBM::create, "font");
			Resources::registerResourceType(ResFontBM::createBM, "bmfc_font");
			Resources::registerResourceType(ResFontBM::createSD, "sdfont");
			Resources::registerResourceType(ResStarlingAtlas::create, "starling");

			checkGLError();
			log::messageln("oxygine initialized");


		}

#if OXYGINE_SDL || EMSCRIPTEN
		Vector2 convertTouch(SDL_Event& ev)
		{
			//log::messageln("convert %.2f %.2f %.2f", ev.tfinger.x, ev.tfinger.y, ev.tfinger.pressure);
			Point size = getDisplaySize();
			return Vector2(ev.tfinger.x * size.x, ev.tfinger.y * size.y);
			//SDL_Touch* inTouch = SDL_GetTouch(ev.tfinger.touchId);
			//return Point(
			//	(int(ev.tfinger.x) * size.x) / inTouch->xres, 
			//	(int(ev.tfinger.y) * size.y) / inTouch->yres);
		}
#endif

#if EMSCRIPTEN
		void *_window = 0;
#endif

#if OXYGINE_SDL || EMSCRIPTEN
		bool active = true;
		bool isActive()
		{
			return active;
		}

		bool focus = true;
		bool hasFocus()
		{
			return focus;
		}

		void* getWindow()
		{
			return _window;
		}
#endif

		void reset()
		{
			log::messageln("core::reset()");
			Restorable::releaseAll();
			Renderer::reset();
			IVideoDriver::instance->reset();
			log::messageln("core::reset() done");
		}

		void restore()
		{
			log::messageln("core::restore()");
			IVideoDriver::instance->restore();
			Renderer::restore();
			Restorable::restoreAll();
			log::messageln("core::restore() done");
		}

		

		void swapDisplayBuffers()
		{
            checkGLError();
#if __S3E__
			IwGLSwapBuffers();
#elif USE_EGL || EMSCRIPTEN
			SDL_GL_SwapBuffers();
			//eglSwapBuffers(eglDisplay, eglSurface);
#elif OXYGINE_SDL
			//if (isActive())
			if (_context)
			{
				int status = SDL_GL_MakeCurrent(_window, _context);
				if (status)
				{
					log::error("SDL_GL_MakeCurrent(): %s", SDL_GetError());
				}
				SDL_GL_SwapWindow(_window);
			}
#endif

			IVideoDriver::instance->getStats(_videoStats);
			IVideoDriver::instance->swapped();

			checkGLError();

			//sleep(1000/50);
		}

		bool update()
		{
			ThreadMessages::message msg;
			while (_threadMessages.peek(msg, true))
			{

			}
#ifdef __S3E__

			s3eDeviceYield(0);
			s3eKeyboardUpdate();
			s3ePointerUpdate();

			bool done = false;

			if (s3eDeviceCheckQuitRequest())
				done = true;

			if (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED)
				done = true;

			return done;
#endif


	#if OXYGINE_SDL || EMSCRIPTEN

			//log::messageln("update");
			Input *input = &Input::instance;
			bool done = false;
			SDL_Event event;
			while (SDL_PollEvent(&event)) 
			{
				Event ev(Input::event_platform);
				ev.userData = &event;
				Input::instance.dispatchEvent(&ev);

				switch(event.type)
				{
				case SDL_QUIT:
					done = true;
					break;
				case SDL_WINDOWEVENT:
					{					
						/*
						if (event.window.event == SDL_WINDOWEVENT_ENTER)
							active = false;
						if (event.window.event == SDL_WINDOWEVENT_LEAVE)
							active = true;
							*/

						if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
							active = false;
						if (event.window.event == SDL_WINDOWEVENT_RESTORED)
							active = true;

						bool newFocus = focus;
						if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
							newFocus = false;
						if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
							newFocus = true;
						if (focus != newFocus)
						{
							focus = newFocus;
#if HANDLE_FOCUS_LOST
							if (focus)							
								lostContext();
							else
								focusLost();

							log::messageln("focus: %d", (int)focus);
							Event ev(focus ? Stage::ACTIVATE : Stage::DEACTIVATE);
							if (getStage())
								getStage()->dispatchEvent(&ev);
#endif							
						}
						//log::messageln("SDL_SYSWMEVENT %d", (int)event.window.event);
						break;
					}
				case SDL_MOUSEWHEEL:
					input->sendPointerWheelEvent(event.wheel.y, &input->_pointerMouse);
					break;
				case SDL_KEYDOWN:
				{
					KeyEvent ev(KeyEvent::KEY_DOWN, &event.key);
					getStage()->dispatchEvent(&ev);
				} break;
				case SDL_KEYUP:
				{
					KeyEvent ev(KeyEvent::KEY_UP, &event.key);
					getStage()->dispatchEvent(&ev);
				} break;
#if SDL_VIDEO_OPENGL
				case SDL_MOUSEMOTION:
					input->sendPointerMotionEvent((float)event.motion.x, (float)event.motion.y, 1.0f, &input->_pointerMouse);
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					{
						MouseButton b = MouseButton_Left;
						switch(event.button.button)
						{
							case 1: b = MouseButton_Left; break;
							case 2: b = MouseButton_Middle; break;
							case 3: b = MouseButton_Right; break;
						}

						input->sendPointerButtonEvent(b, (float)event.button.x, (float)event.button.y, 1.0f, 
							event.type == SDL_MOUSEBUTTONDOWN ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP, &input->_pointerMouse);
					}					
					break;
#else

				case SDL_FINGERMOTION:
					{
						//log::messageln("SDL_FINGERMOTION");
						Vector2 pos = convertTouch(event);
						input->sendPointerMotionEvent(
							pos.x, pos.y, event.tfinger.pressure,
							input->getTouchByID((int)event.tfinger.fingerId));
					}
				
					break;
				case SDL_FINGERDOWN:
				case SDL_FINGERUP:
					{				
						//log::messageln("SDL_FINGER");
						Vector2 pos = convertTouch(event);
						input->sendPointerButtonEvent(
							MouseButton_Touch,
							pos.x, pos.y, event.tfinger.pressure,
							event.type == SDL_FINGERDOWN ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP,
							input->getTouchByID((int)event.tfinger.fingerId));
					}				
					break;
#endif
				}
			}


			return done;
	#elif EMSCRIPTEN
			return false;
	#endif


			log::warning("update not implemented");
			return true;
			/*
#if __FLASHPLAYER__
			//log::messageln("update...");
			//SDL_Delay(3);
			SDL_Delay(3);
			//log::messageln("update...2");
			((VideoDriverStage3D*)IVideoDriver::instance)->present();
			avm2_ui_thunk(handleFrame, NULL);	
			//log::messageln("update...3");
			return false;
#endif
			*/
		}

		void release()
		{
			delete IVideoDriver::instance;
			IVideoDriver::instance = 0;

#ifdef OXYGINE_SDL
			SDL_GL_DeleteContext(_context);
			SDL_DestroyWindow(_window);
			SDL_Quit();
#endif

			spActor temp = Stage::instance;
            if (Stage::instance)
                Stage::instance->cleanup();
			Stage::instance = 0;	

			Input::instance.cleanup();

			Renderer::release();
			DebugActor::release();

			//Winnie::DesroyHeap();
		}

		void execute(const char *str)
		{
#ifdef __S3E__
			s3eOSExecExecute(str, false);
#elif __ANDROID__
			jniBrowse(str);
#else
			OX_ASSERT(!"execute not implemented");
#endif
		}

		void requestQuit()
		{
#ifdef __S3E__
			s3eDeviceRequestQuit();

#endif
#ifdef OXYGINE_SDL
			SDL_Event ev;
			ev.type = SDL_QUIT;
			SDL_PushEvent(&ev);
#endif	
		}

		ThreadMessages& getMainThreadMessages()
		{
			return _threadMessages;
		}

		Point _qtFixedSize(0,0);
		Point getDisplaySize()
		{
	#if __S3E__
			int width = IwGLGetInt(IW_GL_WIDTH);
			int height = IwGLGetInt(IW_GL_HEIGHT);

			int orient = s3eSurfaceGetInt(S3E_SURFACE_DEVICE_ORIENTATION_LOCK);
			if (height > width)
			{
				if (orient == S3E_SURFACE_LANDSCAPE || orient == S3E_SURFACE_LANDSCAPE_FIXED)
					swap(width, height);
			}
			else
			{
				if (orient == S3E_SURFACE_PORTRAIT || orient == S3E_SURFACE_PORTRAIT_FIXED)
					swap(width, height);
			}

			return Point(width, height);
	#endif
	
	#ifdef OXYGINE_QT
			if (!_window)
			{
				return _qtFixedSize;
			}
	#endif

	#if OXYGINE_SDL
			int w = 0;
			int h = 0;
			SDL_GetWindowSize(_window, &w, &h);
			return Point(w, h);
	#endif

	#if EMSCRIPTEN
			return _displaySize;
	#endif

	#if	__FLASHPLAYER__
			return Point(800, 600);
	#endif
			log::warning("getDisplaySize not implemented");
			return Point(0, 0);
		}
	}

	void handleErrorPolicy(error_policy ep, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		
		switch(ep)
		{
		case ep_show_error:
			log::error_va(format, args);
			OX_ASSERT(!"handleErrorPolicy error.");
			break;
		case ep_show_warning:
			log::warning_va(format, args);
			break;
		case ep_ignore_error:
			break;
		default:
			OX_ASSERT(!"not implemented");
		}

		va_end(args);
	}

	int		getTimeUTC()
	{
		return (int)(getTimeUTCMS() / 1000);
	}

	int64	getTimeUTCMS()
	{
#ifdef __S3E__
		return s3eTimerGetUTC();
#elif WIN32
		FILETIME tm;
		GetSystemTimeAsFileTime(&tm);
		int64 t = tm.dwLowDateTime + (int64(tm.dwHighDateTime) << 32);
		int64 utc = (t - 116444736000000000LL)/10000;
		return utc;		
#elif __ANDROID__
		return jniGetTimeUTCMS();
#elif EMSCRIPTEN
		struct timeval tv;
		gettimeofday(&tv, NULL);
		int64 tm = 
			(unsigned long long)(tv.tv_sec) * 1000 +
			(unsigned long long)(tv.tv_usec) / 1000;
		return tm;
#endif
		return getTimeMS();
	}

	bool	isNetworkAvaible()
	{
#ifdef __S3E__
		return s3eSocketGetInt(S3E_SOCKET_NETWORK_AVAILABLE) == 1;
#endif
#ifdef __ANDROID__
		return jniIsNetworkAvailable();
#endif		
		return true;
	}

	std::string		getLanguage()
	{
#ifdef __ANDROID__
		return jniGetLanguage();
#endif		
		return "en";
	}

	void	sleep(timeMS time)
	{
#if __S3E__
		s3eDeviceYield(time);
#elif OXYGINE_SDL
		SDL_Delay(time);
#else
		log::warning("sleep not implemented");
#endif
	}
}
