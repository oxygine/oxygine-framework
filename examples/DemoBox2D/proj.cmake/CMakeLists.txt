cmake_minimum_required (VERSION 2.6)
project (DemoBox2D)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

file(GLOB_RECURSE BOX2DSRC
		../box2d/*.cpp
		../box2d/*.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(DemoBox2D ${BOX2DSRC} ../src/Box2DDebugDraw.cpp ../src/entry_point.cpp ../src/example.cpp  ../src/Box2DDebugDraw.h ../src/example.h )
source_group(box2d FILES ${BOX2DSRC})
include_directories(../box2d)

target_link_libraries(DemoBox2D ${OXYGINE_CORE_LIBS})

if (WIN32) #disable console mode for VC++
	set_target_properties(DemoBox2D PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)