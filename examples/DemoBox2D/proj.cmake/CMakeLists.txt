cmake_minimum_required (VERSION 2.6)
project (DemoBox2D)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

file(GLOB_RECURSE BOX2DSRC
		../box2d/*.cpp
		../box2d/*.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

add_executable(DemoBox2D ${BOX2DSRC} ../src/Box2DDebugDraw.cpp ../src/main.cpp ../src/example.cpp  ../src/Box2DDebugDraw.h ../src/example.h )
source_group(box2d FILES ${BOX2DSRC})
include_directories(../box2d)

target_link_libraries(DemoBox2D ${OXYGINE_CORE_LIBS})




if (WIN32) #disable console mode for VC++
	set_target_properties(DemoBox2D PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)



if (EMSCRIPTEN)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648")
	em_link_pre_js(DemoBox2D  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)