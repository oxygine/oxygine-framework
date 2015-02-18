#include "oxygine.h"
#include <stdio.h>
#include "Renderer.h"
#include "log.h"
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
#include "KeyEvent.h"
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
#include <sys/time.h>
#include <emscripten.h>
#include <SDL.h>
#include <SDL_compat.h>
#include <SDL_events.h>
#elif __ANDROID__
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

#define LOST_RESET_CONTEXT 0


namespace oxygine
{
	namespace file
	{
		void init(const char *company, const char *app);
	}


	IVideoDriver::Stats _videoStats;

	static ThreadMessages _threadMessages;
	static ThreadMessages _uiMessages;
	Mutex mutexAlloc;

#if EMSCRIPTEN
	static Point _displaySize(0, 0);
	static void *_window = 0;
#endif


#ifdef OXYGINE_SDL
	static SDL_Window *_window = 0;
	static SDL_GLContext _context = 0;
#endif

	static core::init_desc desc;
    Point _qtFixedSize(0, 0);


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

		Input::instance.sendPointerButtonEvent(getStage(), b, (float)ev->m_x, (float)ev->m_y, 1.0f, type, &Input::instance._pointerMouse);

		return 0;
	}

	int32 pointerMotionEvent(void *sysData, void *u)
	{
		s3ePointerMotionEvent *ev = (s3ePointerMotionEvent*)sysData;

		Input::instance.sendPointerMotionEvent(getStage(), (float)ev->m_x, (float)ev->m_y, 1.0f, &Input::instance._pointerMouse);
		return 0;
	}

	int32 pointerTouchEvent(void *sysData, void *u)
	{
		Input *This = (Input*)u;
		s3ePointerTouchEvent *ev = (s3ePointerTouchEvent*)sysData;
		int id = ev->m_TouchID + 1;

		Input::instance.sendPointerButtonEvent(getStage(), MouseButton_Touch, (float)ev->m_x, (float)ev->m_y, 1.0f, ev->m_Pressed ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP, Input::instance.getTouchByIndex(id));

		return 0;
	}

	int32 pointerTouchMotionEvent(void *sysData, void *u)
	{
		Input *This = (Input*)u;
		s3ePointerTouchMotionEvent *ev = (s3ePointerTouchMotionEvent*)sysData;
		int id = ev->m_TouchID + 1;

		Input::instance.sendPointerMotionEvent(getStage(), (float)ev->m_x, (float)ev->m_y, 1.0f, Input::instance.getTouchByIndex(id));

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

	namespace core
	{
		void focusLost()
		{
			if (!LOST_RESET_CONTEXT)
				return;

#if OXYGINE_SDL
			log::messageln("focus lost");
			SDL_GL_DeleteContext(_context);
			_context = 0;
#endif

		}

		void focusAcquired()
		{
			if (!LOST_RESET_CONTEXT)
				return;

#if OXYGINE_SDL			
			log::messageln("lost context");
			if (!_context)
			{
				_context = SDL_GL_CreateContext(_window);
				initGLExtensions(SDL_GL_GetProcAddress);
			}
#endif			
		}

#ifdef EMSCRIPTEN
		void SDL_handleEvent(SDL_Event &event, bool &done);
		int SDL_eventsHandler(void *, SDL_Event *e)
		{
			bool done = false;
			SDL_handleEvent(*e, done);
			return 0;
		}
#endif


		void updateUIMessages()
		{
			ThreadMessages::message msg;
			while (_uiMessages.peek(msg, true))	{}
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
#elif EMSCRIPTEN
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

			emscripten_SDL_SetEventHandler(SDL_eventsHandler, 0);
#elif OXYGINE_SDL

			log::messageln("SDL build");
			

			SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");

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
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

			int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

#if TARGET_OS_IPHONE
			flags |= SDL_WINDOW_BORDERLESS;
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
			

			//SDL_SetEventFilter(eventsFilter, 0);

#endif
			file::init(desc.companyName, desc.appName);
			init2();
		}


		void init2()
		{
#ifdef OXYGINE_QT
            setlocale(LC_ALL, "POSIX");
#endif

#ifdef OXYGINE_SDL
			int missing = initGLExtensions(SDL_GL_GetProcAddress);
#endif

			Point size = getDisplaySize();
			log::messageln("display size: %d %d", size.x, size.y);


#if __S3E__
			int glversion = s3eGLGetInt(S3E_GL_VERSION);
			int major_gl = glversion >> 8;
			OX_ASSERT( major_gl == 2 && "gl version should be 2");			
			IVideoDriver::instance = new VideoDriverGLES20();
#endif

			IVideoDriver::instance = new VideoDriverGLES20();


			CHECKGL();

			IVideoDriver::instance->setDefaultSettings();

			CHECKGL();

			Renderer::initialize();

			Resources::registerResourceType(ResAtlas::create, "atlas");
			Resources::registerResourceType(ResBuffer::create, "buffer");
			Resources::registerResourceType(ResFontBM::create, "font");
			Resources::registerResourceType(ResFontBM::createBM, "bmfc_font");
			Resources::registerResourceType(ResFontBM::createSD, "sdfont");
			Resources::registerResourceType(ResStarlingAtlas::create, "starling");

			CHECKGL();
			log::messageln("oxygine initialized");
		}

#if OXYGINE_SDL || EMSCRIPTEN
		Vector2 convertTouch(SDL_Event& ev)
		{
			Point size = getDisplaySize();
			return Vector2(ev.tfinger.x * size.x, ev.tfinger.y * size.y);
		}
#endif

		bool active = true;
		bool focus = true;


		bool isActive()
		{
			return active;
		}

		bool hasFocus()
		{
			return focus;
		}


#if OXYGINE_SDL
		SDL_GLContext getGLContext()
		{
			return _context;
		}

		SDL_Window* getWindow()
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


		bool beginRendering(window w)
		{
#ifdef OXYGINE_SDL
			SDL_Window *wnd = w;
			if (!wnd)
				wnd = _window;			
			SDL_GL_MakeCurrent(wnd, _context);
#endif			

			CHECKGL();

			return Renderer::isReady();
		}

		void swapDisplayBuffers(window w)
		{
#if OXYGINE_SDL
			if (!_context)
				return;
#endif

			CHECKGL();
#if __S3E__
			IwGLSwapBuffers();
#elif EMSCRIPTEN
			SDL_GL_SwapBuffers();
#elif OXYGINE_SDL
			SDL_Window *wnd = w;
			if (!wnd)
				wnd = _window;
			if (_context)
			{
				int status = SDL_GL_MakeCurrent(wnd, _context);
				if (status)
				{
					log::error("SDL_GL_MakeCurrent(): %s", SDL_GetError());
				}
				SDL_GL_SwapWindow(wnd);
			}
#endif

			IVideoDriver::instance->getStats(_videoStats);
			IVideoDriver::instance->swapped();

			CHECKGL();

			//sleep(1000/50);
		}

#ifndef __S3E__
		void SDL_handleEvent(SDL_Event &event, bool &done)
		{
			Input *input = &Input::instance;


#ifndef EMSCRIPTEN
			SDL_Window *wnd = SDL_GetWindowFromID(event.window.windowID);			
			void *data = SDL_GetWindowData(wnd, "_");
			spStage stage = (Stage*)data;
#else
			spStage stage = getStage();
#endif
			if (!stage)
				stage = getStage();

			Event ev(Input::event_platform);
			ev.userData = &event;
			Input::instance.dispatchEvent(&ev);

			switch (event.type)
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
						focusAcquired();

					log::messageln("focus: %d", (int)focus);
					Event ev(focus ? Stage::ACTIVATE : Stage::DEACTIVATE);
					if (stage)
						stage->dispatchEvent(&ev);

					if (!focus)
						focusLost();
#endif							
				}
				//log::messageln("SDL_SYSWMEVENT %d", (int)event.window.event);
				break;
			}
			case SDL_MOUSEWHEEL:
				input->sendPointerWheelEvent(stage, event.wheel.y, &input->_pointerMouse);
				break;
			case SDL_KEYDOWN:
			{
				KeyEvent ev(KeyEvent::KEY_DOWN, &event.key);
				stage->dispatchEvent(&ev);
			} break;
			case SDL_KEYUP:
			{
				KeyEvent ev(KeyEvent::KEY_UP, &event.key);
				stage->dispatchEvent(&ev);
			} break;
#if SDL_VIDEO_OPENGL
			case SDL_MOUSEMOTION:
				input->sendPointerMotionEvent(stage, (float)event.motion.x, (float)event.motion.y, 1.0f, &input->_pointerMouse);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				MouseButton b = MouseButton_Left;
				switch (event.button.button)
				{
				case 1: b = MouseButton_Left; break;
				case 2: b = MouseButton_Middle; break;
				case 3: b = MouseButton_Right; break;
				}

				input->sendPointerButtonEvent(stage, b, (float)event.button.x, (float)event.button.y, 1.0f,
					event.type == SDL_MOUSEBUTTONDOWN ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP, &input->_pointerMouse);
			}
				break;
#else

			case SDL_FINGERMOTION:
			{
				//log::messageln("SDL_FINGERMOTION");
				Vector2 pos = convertTouch(event);
				input->sendPointerMotionEvent(stage,
					pos.x, pos.y, event.tfinger.pressure,
					input->getTouchByID((int)event.tfinger.fingerId));
			}

				break;
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			{
				//log::messageln("SDL_FINGER");
				Vector2 pos = convertTouch(event);
				input->sendPointerButtonEvent(stage, 
					MouseButton_Touch,
					pos.x, pos.y, event.tfinger.pressure,
					event.type == SDL_FINGERDOWN ? TouchEvent::TOUCH_DOWN : TouchEvent::TOUCH_UP,
					input->getTouchByID((int)event.tfinger.fingerId));
			}
				break;
#endif
			}

		}
#endif

		bool update()
		{
			ThreadMessages::message msg;
			while (_threadMessages.peek(msg, true))	{}

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

			bool done = false;
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
#if !EMSCRIPTEN //emscripten handled events from callback
				SDL_handleEvent(event, done);
#endif
			}

			return done;
#endif

			log::warning("update not implemented");
			return true;
		}

		void release()
		{
			delete IVideoDriver::instance;
			IVideoDriver::instance = 0;
			
			Input::instance.cleanup();

			Renderer::release();

			DebugActor::release();

			if (Stage::instance)
				Stage::instance->cleanup();
			Stage::instance = 0;
#if OXYGINE_SDL
			SDL_GL_DeleteContext(_context);
			SDL_DestroyWindow(_window);
			SDL_Quit();
#endif
		}

		void execute(const char *str)
		{
#ifdef __S3E__
			s3eOSExecExecute(str, false);
#elif __ANDROID__
			jniBrowse(str);
#elif EMSCRIPTEN
			EM_ASM_INT({
				var url = Pointer_stringify($0);
				window.open(url, '_blank');
			}, str);
#else
			OX_ASSERT(!"execute not implemented");
#endif
		}

		void requestQuit()
		{
#ifdef __S3E__
			s3eDeviceRequestQuit();
#elif OXYGINE_SDL
			SDL_Event ev;
			ev.type = SDL_QUIT;
			SDL_PushEvent(&ev);
#endif	
		}

		ThreadMessages& getMainThreadMessages()
		{
			return _threadMessages;
		}

		ThreadMessages& getUiThreadMessages()
		{
			return _uiMessages;
		}

		
		Point getDisplaySize()
		{
#if __S3E__
			int width = IwGLGetInt(IW_GL_WIDTH);
			int height = IwGLGetInt(IW_GL_HEIGHT);

			int orient = s3eSurfaceGetInt(S3E_SURFACE_DEVICE_ORIENTATION_LOCK);
			if (height > width)
			{
				if (orient == S3E_SURFACE_LANDSCAPE || orient == S3E_SURFACE_LANDSCAPE_FIXED)
					std::swap(width, height);
			}
			else
			{
				if (orient == S3E_SURFACE_PORTRAIT || orient == S3E_SURFACE_PORTRAIT_FIXED)
					std::swap(width, height);
			}

			return Point(width, height);
#elif OXYGINE_QT
			if (!_window)
			{
				return _qtFixedSize;
			}
#elif OXYGINE_SDL
			int w = 0;
			int h = 0;
			SDL_GetWindowSize(_window, &w, &h);
			return Point(w, h);
#elif EMSCRIPTEN
			return _displaySize;
#else
			log::warning("getDisplaySize not implemented");
			return Point(0, 0);
#endif
		}
		}

	void handleErrorPolicy(error_policy ep, const char *format, ...)
	{
		va_list args;
		va_start(args, format);

		switch (ep)
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

	bool	isNetworkAvaible()
	{
#if __S3E__
		return s3eSocketGetInt(S3E_SOCKET_NETWORK_AVAILABLE) == 1;
#elif __ANDROID__
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
