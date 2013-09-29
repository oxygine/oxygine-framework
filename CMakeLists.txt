cmake_minimum_required (VERSION 2.6)
project (OXYGINE)

#SDL2
set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../sdl/include)
set(SDL2_LIBRARIES sdl2)

set(OXYGINE_DEFINITIONS	
	-D_CRT_SECURE_NO_WARNINGS)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/greenlets/src/*.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/oxygine/greenlets/src/)


set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/win32)

set(OXYGINE_ROOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/src)
set(FOLDERS . closure minizip core core/gl dev_tools minizip math pugixml res text_utils utils winnie_alloc)
foreach(F ${FOLDERS})
	file(GLOB FLS #RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/
		${OXYGINE_ROOT_SRC}/${F}/*.cpp
		${OXYGINE_ROOT_SRC}/${F}/*.c
		${OXYGINE_ROOT_SRC}/${F}/*.h)
	set(SOURCES ${SOURCES} ${FLS})
	source_group(${F} FILES ${FLS})
endforeach(F)



set(OXYGINE_INCLUDE_DIRS 
	${OXYGINE_ROOT_SRC}
	${SDL2_INCLUDE_DIRS}
	${THIRD_PARTY}/glew
	${THIRD_PARTY}/zlib)

include_directories(${OXYGINE_INCLUDE_DIRS})


find_package(OPENGL)
include_directories(
		${THIRD_PARTY}/libjpeg
		${THIRD_PARTY}/libpng
		)

add_definitions(${OXYGINE_DEFINITIONS})
add_library(oxygine-framework STATIC ${SOURCES})


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
	oxygine-framework
	glew32
)


set(OXYGINE_LIBRARY_DIRS
	${OXYGINE_LIBRARY_DIRS}
	PARENT_SCOPE)

set(OXYGINE_CORE_LIBS	
	${OXYGINE_CORE_LIBS} 
	PARENT_SCOPE)

set(OXYGINE_DEFINITIONS
	${OXYGINE_DEFINITIONS}
	PARENT_SCOPE)

set(OXYGINE_INCLUDE_DIRS
	${OXYGINE_INCLUDE_DIRS}
	PARENT_SCOPE)

