cmake_minimum_required (VERSION 2.6)
project (DemoBox2D)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

file(GLOB_RECURSE BOX2DSRC
		../box2d/*.cpp
		../box2d/*.h)

include_directories(../box2d)

add_executable(DemoBox2D ${BOX2DSRC} ../src/Box2DDebugDraw.cpp ../src/entry_point.cpp ../src/example.cpp  ../src/Box2DDebugDraw.h ../src/example.h )



if (CMAKE_BUILD_TYPE STREQUAL Debug)
	SET(linkFlags "-g4 ")
else() # Either MinSizeRel, RelWithDebInfo or Release, all which run with optimizations enabled.
	SET(linkFlags "-O2 ")
endif()

set(linkFlags "${linkFlags} -s NO_EXIT_RUNTIME=1 -s FULL_ES2=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -Wno-warn-absolute-paths -s TOTAL_MEMORY=64435456")

set_target_properties(DemoBox2D PROPERTIES LINK_FLAGS "${linkFlags}")

target_link_libraries(DemoBox2D ${OXYGINE_CORE_LIBS})
em_link_pre_js(DemoBox2D ${CMAKE_CURRENT_SOURCE_DIR}/data.js)