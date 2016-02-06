cmake_minimum_required (VERSION 2.6)
project (OXYGINE)


set(OX_HAVE_LIBJPEG 0)
set(OX_HAVE_LIBPNG 0)
set(OX_HAVE_LIBCURL 0)
set(OX_HAVE_HTTP 0)
set(OX_USE_SDL2 1)


find_package(OpenGL)

if (EMSCRIPTEN)
	#don't need SDL2
elseif (WIN32)	
	#hardcoded path to SDL2 on windows
	set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../SDL/include)
else(WIN32)
	find_path(SDL2_INCLUDE_DIRS NAMES SDL2/SDL.h)
	message(STATUS ${SDL2_INCLUDE_DIRS_FOUND})

	if (SDL2_INCLUDE_DIRS)		
		set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS}/SDL2)
		message(STATUS "found")		
	else()
		message(STATUS "SDL not found")
		set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../SDL/include)					
	endif()

	find_package(CURL)
endif(EMSCRIPTEN)


set(OXYGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/oxygine)
set(OXYGINE_SRC ${OXYGINE_ROOT}/src)

set(FOLDERS src src/closure src/minizip src/core
			src/core/gl src/dev_tools src/minizip 
			src/math src/pugixml src/json src/res 
			src/text_utils src/utils src/winnie_alloc)


if (EMSCRIPTEN)
	set(PLATFORM emscripten)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux") 	
	set(PLATFORM linux)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(PLATFORM ios)
elseif(MSVC)
	set(PLATFORM win32)
elseif(MINGW)	
	set(PLATFORM win32_mingw)
endif()


set(THIRD_PARTY ${OXYGINE_ROOT}/third_party/${PLATFORM})



if (EMSCRIPTEN)
	set(OX_HAVE_LIBPNG 1)
	set(OX_HAVE_HTTP 1)	
	set(OX_USE_SDL2 0)

	set(SOURCES ${OXYGINE_SRC}/core/emscripten/HttpRequestEmscriptenTask.cpp)


	file(GLOB OXYGINE_JS_LIBRARIES ${OXYGINE_SRC}/core/emscripten/*.js)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux") 	
	
	set(OX_HAVE_LIBJPEG 1)
	set(OX_HAVE_LIBPNG 1)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
elseif(MSVC)

	set(OX_HAVE_LIBJPEG 1)
	set(OX_HAVE_LIBPNG 1)	
	set(OX_HAVE_LIBCURL 1)
	set(OX_HAVE_HTTP 1)

	set(libprefix lib)

	set(OX_DEFINITIONS	${OX_DEFINITIONS} -D_CRT_SECURE_NO_WARNINGS)

elseif(MINGW)	

	set(libprefix lib)

	set(OX_HAVE_LIBPNG 1)	
	set(OX_HAVE_LIBCURL 1)
	set(OX_HAVE_HTTP 1)

endif()

if (OX_HAVE_LIBCURL)	
	set(FOLDERS ${FOLDERS} src/core/curl)
	include_directories(${THIRD_PARTY}/curl/)
	set(OX_DEFINITIONS ${OX_DEFINITIONS} -DOX_HAVE_LIBCURL)
endif(OX_HAVE_LIBCURL)	


if (NOT OX_HAVE_HTTP)
	set(OX_DEFINITIONS ${OX_DEFINITIONS} -DOX_NO_HTTP)
endif(NOT OX_HAVE_HTTP)



foreach(ITEM ${FOLDERS})
	file(GLOB FLS
		${OXYGINE_ROOT}/${ITEM}/*.cpp
		${OXYGINE_ROOT}/${ITEM}/*.c
		${OXYGINE_ROOT}/${ITEM}/*.h)
	set(SOURCES ${SOURCES} ${FLS})	
	string(REPLACE / \\ SGROUP ${ITEM})
	source_group(${SGROUP} FILES ${FLS})
endforeach(ITEM)


set(OXYGINE_INCLUDE_DIRS 
	${OXYGINE_SRC}
	${THIRD_PARTY}/pthreads/include/
	${THIRD_PARTY}/zlib)


set(OXYGINE_LIBRARY_DIRS
	${OXYGINE_LIBRARY_DIRS}
	${OXYGINE_SOURCE_DIR}/libs
	${THIRD_PARTY}/libraries)


if (FORCE_GLES)
	set(OPENGL_LIBRARIES libGLESv2.lib)
endif(FORCE_GLES)


if (MINGW) 
	set(CORE_LIBS ${CORE_LIBS}	mingw32)
endif(MINGW)


set(CORE_LIBS 
	${CORE_LIBS}
	oxygine-framework	
	${OPENGL_LIBRARIES}		
)


if (OX_USE_SDL2)
	set(CORE_LIBS ${CORE_LIBS}
		SDL2main SDL2)
	set(OXYGINE_INCLUDE_DIRS ${OXYGINE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
endif(OX_USE_SDL2)	


if (WIN32)
	set(CORE_LIBS ${CORE_LIBS}
		pthreadVCE2
		libcurl_imp
		ws2_32)
elseif(EMSCRIPTEN)	
else(WIN32)
	set(CORE_LIBS ${CORE_LIBS} pthread)
endif(WIN32)



if (OX_HAVE_LIBPNG)		
	set(OX_DEFINITIONS	${OX_DEFINITIONS} -DOX_HAVE_LIBPNG)
	include_directories(${THIRD_PARTY}/libpng)
	set(LIBPNG ${libprefix}png)

	if (MSVC)		
		if (MSVC_VERSION EQUAL "1900")
			set(LIBPNG ${LIBPNG}-2015)
		endif()
	elseif(EMSCRIPTEN)
		set(LIBPNG libz libpng16)
	endif()	

	set(CORE_LIBS ${CORE_LIBS} ${LIBPNG})
endif(OX_HAVE_LIBPNG)


if (OX_HAVE_LIBJPEG)	
	set(OX_DEFINITIONS	${OX_DEFINITIONS} -DOX_HAVE_LIBJPEG)
	include_directories(${THIRD_PARTY}/libjpeg)
	set(LIBJPEG ${libprefix}jpeg)
	
	if (MSVC)
		if (MSVC_VERSION EQUAL "1900")
			set(LIBJPEG ${LIBJPEG}-2015)
		endif()	
	endif()

	set(CORE_LIBS ${CORE_LIBS} ${LIBJPEG})
endif(OX_HAVE_LIBJPEG)


if (NOT EMSCRIPTEN)
	set(CORE_LIBS ${CORE_LIBS}
		${libprefix}z${libprefix})
endif(NOT EMSCRIPTEN)


if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
endif(NOT MSVC)


add_definitions(${OX_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
add_library(oxygine-framework STATIC ${SOURCES})


set(OXYGINE_LIBRARY_DIRS
	${OXYGINE_LIBRARY_DIRS}
	PARENT_SCOPE)

set(OXYGINE_CORE_LIBS	
	${CORE_LIBS} 
	PARENT_SCOPE)

set(OXYGINE_DEFINITIONS
	${OX_DEFINITIONS}
	PARENT_SCOPE)

set(OXYGINE_INCLUDE_DIRS
	${OXYGINE_INCLUDE_DIRS}
	PARENT_SCOPE)

set(OXYGINE_JS_LIBRARIES
	${OXYGINE_JS_LIBRARIES}
	PARENT_SCOPE)

message(STATUS "SDL includes: ${SDL2_INCLUDE_DIRS}")
message(STATUS "Libs:     ${CORE_LIBS}")
message(STATUS "Platform: ${PLATFORM}")

set(CMAKE_INSTALL_PREFIX ../libs)
install(TARGETS oxygine-framework CONFIGURATIONS Debug DESTINATION ./debug)
install(TARGETS oxygine-framework CONFIGURATIONS Release DESTINATION ./release)
