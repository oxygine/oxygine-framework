cmake_minimum_required (VERSION 2.6)
project (OXYGINE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

if (WIN32)
	set(OX_DEFINITIONS	${OX_DEFINITIONS}
		-D_CRT_SECURE_NO_WARNINGS)
else(WIN32)
	find_package(CURL)
endif(WIN32)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(OX_HAVE_LIBJPEG 1)
set(OX_HAVE_LIBPNG 1)

if (EMSCRIPTEN)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(OX_HAVE_LIBJPEG 0)
	set(OX_HAVE_LIBPNG 1)
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/emscripten)
	set(SDL2_INCLUDE_DIRS "")
	file(GLOB OXYGINE_JS_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/src/core/emscripten/*.js)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux") 	
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/linux)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/ios)
elseif(MSVC)
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/win32)	
	set(libprefix lib)
elseif(MINGW)	
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/win32_mingw)
	set(libprefix lib)
	set(OX_HAVE_LIBJPEG 0)	
endif()
 

if (EMSCRIPTEN)
	set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/src/core/emscripten/HttpRequestEmscriptenTask.cpp)
endif(EMSCRIPTEN)


set(OXYGINE_ROOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/src)
set(FOLDERS . closure minizip core core/gl dev_tools minizip math pugixml json res text_utils utils winnie_alloc)
if (WIN32)
	set(FOLDERS ${FOLDERS} core/curl)
	include_directories(${THIRD_PARTY}/curl/)
	set(OX_HAVE_LIBCURL 1)
elseif(EMSCRIPTEN)
	set(OX_HAVE_LIBCURL 0)
else(WIN32)
	include_directories(${CURL_INCLUDE_DIRS})
	set(CORE_LIBS ${CORE_LIBS} ${CURL_LIBRARIES})
	#set(FOLDERS ${FOLDERS} core/curl)
	set(OX_HAVE_LIBCURL 0)
	set(OX_NO_HTTP 1)
	set(OX_DEFINITIONS ${OX_DEFINITIONS} -DOX_NO_HTTP)
endif(WIN32)

foreach(F ${FOLDERS})
	file(GLOB FLS #RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/
		${OXYGINE_ROOT_SRC}/${F}/*.cpp
		${OXYGINE_ROOT_SRC}/${F}/*.c
		${OXYGINE_ROOT_SRC}/${F}/*.h)
	set(SOURCES ${SOURCES} ${FLS})
	source_group(${F} FILES ${FLS})
endforeach(F)


find_package(OpenGL REQUIRED)

#add png library
if (OX_HAVE_LIBPNG)
	find_package(PNG REQUIRED)
	set (CORE_LIBS
		${CORE_LIBS}
		${PNG_LIBRARIES}
	)
	include_directories(
		${PNG_INCLUDE_DIR}
	)
	set(OX_DEFINITIONS	${OX_DEFINITIONS}
		-DOX_HAVE_LIBPNG)
endif(OX_HAVE_LIBPNG)

#add jpeg library
if (OX_HAVE_LIBJPEG)
	find_package(JPEG REQUIRED)
	set (CORE_LIBS
		${CORE_LIBS}
		${JPEG_LIBRARIES}
	)
	include_directories(
		${JPEG_INCLUDE_DIR}
	)
	set(OX_DEFINITIONS	${OX_DEFINITIONS}
		-DOX_HAVE_LIBJPEG)
endif(OX_HAVE_LIBJPEG)

set(OXYGINE_INCLUDE_DIRS 
	${OXYGINE_ROOT_SRC}
	${SDL2_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
	${JPEG_INCLUDE_DIR}
	${THIRD_PARTY}/pthreads/include/
	${THIRD_PARTY}/zlib)


include_directories(${OXYGINE_INCLUDE_DIRS})


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
	${SDL2_LIBRARY}

)

if (WIN32)
	set(CORE_LIBS		
		${CORE_LIBS}
		pthreadVCE2
		libcurl_imp
		ws2_32
		)

elseif(EMSCRIPTEN)

else(WIN32)
	set(CORE_LIBS
		${CORE_LIBS}
		pthread	
	)
endif(WIN32)


#add z library
if (EMSCRIPTEN)
	set(CORE_LIBS
		${CORE_LIBS}
		libz
		libpng16
	)
else(EMSCRIPTEN)
	set(CORE_LIBS
		${CORE_LIBS}
		${libprefix}z${libprefix})
endif(EMSCRIPTEN)


#message(STATUS ${CORE_LIBS})


if (OX_HAVE_LIBCURL)
	set(OX_DEFINITIONS ${OX_DEFINITIONS}
		-DOX_HAVE_LIBCURL)
endif(OX_HAVE_LIBCURL)

add_definitions(${OX_DEFINITIONS})
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



set(CMAKE_INSTALL_PREFIX ../libs)
install(TARGETS oxygine-framework CONFIGURATIONS Debug DESTINATION ./debug)
install(TARGETS oxygine-framework CONFIGURATIONS Release DESTINATION ./release)

