cmake_minimum_required (VERSION 2.6)
project (OXYGINE)

if (MSVC)
	set(OXYGINE_DEFINITIONS	
		-D_CRT_SECURE_NO_WARNINGS)
	set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../SDL/include)
else(MSVC)
	#set(OXYGINE_DEFINITIONS	
	#	-D_REENTRANT) #linux pthreads
	find_path(SDL2_INCLUDE_DIRS NAMES SDL2/SDL.h)
	message(STATUS ${SDL2_INCLUDE_DIRS_FOUND})

	if (SDL2_INCLUDE_DIRS)		
		set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS}/SDL2)
		message(STATUS "found")		
	else()
		message(STATUS "SDL notfound")
		set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../SDL/include)					
	endif()
endif(MSVC)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/greenlets/src/greenlet.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/oxygine/greenlets/src/)


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/linux)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/ios)
else()
	set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/oxygine/third_party/win32)
endif()


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
	${THIRD_PARTY}/pthreads/include/
	${THIRD_PARTY}/zlib)

include_directories(${OXYGINE_INCLUDE_DIRS})


find_package(OpenGL)
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

set(OXYGINE_CORE_LIBS 
	${OXYGINE_CORE_LIBS}
	oxygine-framework
	SDL2
	SDL2main	
	${OPENGL_LIBRARIES}		
)


if (MSVC)
	set(OXYGINE_CORE_LIBS
		${OXYGINE_CORE_LIBS}
		glew32
		pthreadVCE2
		libpng
		libjpeg
		libzlib)
else(MSVC)
	set(OXYGINE_CORE_LIBS
		${OXYGINE_CORE_LIBS}
		z
		pthread
		png
		jpeg		
	)
endif(MSVC)

#foreach(lib ${STATIC_LIB_NAMES})
#	set(OXYGINE_CORE_LIBS 
#		${OXYGINE_CORE_LIBS}
#		${lib}${CMAKE_STATIC_LIBRARY_SUFFIX})
#endforeach(lib)


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

set(CMAKE_INSTALL_PREFIX ../libs)
install(TARGETS oxygine-framework CONFIGURATIONS Debug DESTINATION ./debug)
install(TARGETS oxygine-framework CONFIGURATIONS Release DESTINATION ./release)

