cmake_minimum_required (VERSION 2.6)
project (DEMOBOX2D)

file(GLOB_RECURSE BOX2DSRC
		box2d/*.cpp
		box2d/*.h)

include_directories(box2d)

add_executable(demobox2d 
	${BOX2DSRC}
	src/entry_point.cpp
	src/example.h 
	src/example.cpp)

source_group(box2d FILES ${BOX2DSRC})

target_link_libraries(demobox2d ${OXYGINE_CORE_LIBS})