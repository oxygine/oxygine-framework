cmake_minimum_required (VERSION 2.6)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project (OXYGINE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OXYGINE_SOURCE_DIR}/cmake/)


set(OXYGINE_ROOT_SRC ${OXYGINE_SOURCE_DIR}/oxygine/src/)

set(FLASCC ${CYGWIN})

if (FLASCC)
	message(STATUS fcc)
else(FLASCC)
	message(STATUS notfcc)
	set(OXYGINE_DEFINITIONS	
		-DOXYGINE_SDL
		-D_CRT_SECURE_NO_WARNINGS)
	find_package(SDL2)
	file(GLOB OXYGINE_SOURCES
		${OXYGINE_SOURCE_DIR}/oxygine/greenlets/src/*.c)
endif(FLASCC) 



set(INCLUDED_FOLDERS 
	.
	pugixml
	closure
	core
	math	
	utils	
	res)

set(NOT_INCLUDED_FOLDERS
	dev_tools
	text_utils	
	winnie_alloc)

if (FLASCC)
	set(NOT_INCLUDED_FOLDERS 
		${NOT_INCLUDED_FOLDERS}
		core/stage3d)
else(FLASCC)
	set(NOT_INCLUDED_FOLDERS 
		${NOT_INCLUDED_FOLDERS}
		core/gl)
endif(FLASCC)

set(FOLDERS ${INCLUDED_FOLDERS} ${NOT_INCLUDED_FOLDERS})
set(OXYGINE_INCLUDE_DIRS ${OXYGINE_ROOT_SRC})

foreach(F ${FOLDERS})
	file(GLOB CPP ${OXYGINE_ROOT_SRC}/${F}/*.cpp)
	file(GLOB HDR ${OXYGINE_ROOT_SRC}/${F}/*.h)

	set(OXYGINE_SOURCES ${OXYGINE_SOURCES} ${CPP} ${HDR})
	list(FIND NOT_INCLUDED_FOLDERS ${F} DONT_INCLUDE)

	#if (DONT_INCLUDE EQUAL -1)		
	#	set(OXYGINE_INCLUDE_DIRS ${OXYGINE_INCLUDE_DIRS} ${OXYGINE_ROOT_SRC}/${F})
	#endif()

	source_group(src\\${F} FILES ${CPP} ${HDR})
endforeach(F)


if (WIN32)
	find_package(OPENGL)
	set(THIRD_PARTY ${OXYGINE_SOURCE_DIR}/oxygine/third_party/win32)
endif(WIN32)

if (APPLE)
	find_library(OpenGL_LIBRARY OpenGL)
	mark_as_advanced(OpenGL_LIBRARY)
	
	set(THIRD_PARTY ${OXYGINE_SOURCE_DIR}/oxygine/third_party/macosx)
	set(OXYGINE_INCLUDE_DIRS ${OXYGINE_INCLUDE_DIRS} /System/Library/Frameworks)	
	set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES} ${OpenGL_LIBRARY})
endif(APPLE)


include_directories(
	${SDL2_INCLUDE_DIRS}
	${OXYGINE_SOURCE_DIR}/oxygine/greenlets/src/
	${OXYGINE_INCLUDE_DIRS}
)

if (THIRD_PARTY)
	include_directories(
		${THIRD_PARTY}/libjpeg
		${THIRD_PARTY}/libpng
		${THIRD_PARTY}/zlib
		${THIRD_PARTY}/glew
	)
else(THIRD_PARTY)	
	include_directories(
		${FLASCC_SDK_DIR}/usr/include/libpng15
	)
	set(OXYGINE_LIBRARY_DIRS
		${OXYGINE_LIBRARY_DIRS}
		${FLASCC_SDK_DIR}/usr/lib)
endif(THIRD_PARTY)


set(OXYGINE_INCLUDE_DIRS 
	${FLASCC_SDK_DIR}/usr/include/libpng15
	${THIRD_PARTY}/glew
	${OXYGINE_INCLUDE_DIRS} 
	${THIRD_PARTY}/zlib
	${SDL2_INCLUDE_DIRS}
	PARENT_SCOPE)

add_definitions(${OXYGINE_DEFINITIONS})


add_library(oxygine STATIC ${OXYGINE_SOURCES})



set(OXYGINE_LIBRARY_DIRS
	${OXYGINE_LIBRARY_DIRS}
	${OXYGINE_SOURCE_DIR}/libs
	${THIRD_PARTY}/libraries)


set(STATIC_LIB_NAMES 
	libpng
	libjpeg
	libzlib
	${CMAKE_STATIC_LIBRARY_PREFIX}sdl2
	${CMAKE_STATIC_LIBRARY_PREFIX}sdl2main
	)


foreach(lib ${STATIC_LIB_NAMES})
	set(OXYGINE_CORE_LIBS 
		${OXYGINE_CORE_LIBS}
		${lib}${CMAKE_STATIC_LIBRARY_SUFFIX})
endforeach(lib)


set(OXYGINE_CORE_LIBS 
	${OXYGINE_CORE_LIBS}
	${OPENGL_LIBRARIES}
	oxygine
)


if (WIN32)
	set(OXYGINE_CORE_LIBS 
		${OXYGINE_CORE_LIBS}
		glew32)
endif(WIN32)

if (APPLE)
endif(APPLE)


#export variables to parent scope

set(OXYGINE_LIBRARY_DIRS
	${OXYGINE_LIBRARY_DIRS}
	PARENT_SCOPE)

set(OXYGINE_CORE_LIBS	
	${OXYGINE_CORE_LIBS} 
	PARENT_SCOPE)

set(OXYGINE_DEFINITIONS
	${OXYGINE_DEFINITIONS}
	PARENT_SCOPE)

set(OXYGINE_SOURCES
	${OXYGINE_SOURCES}
	PARENT_SCOPE)


