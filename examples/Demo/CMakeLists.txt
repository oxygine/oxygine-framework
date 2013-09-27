cmake_minimum_required (VERSION 2.6)
project (DEMO)

add_executable(demo 
	src/entry_point.cpp
	src/example.h	
	src/example.cpp
	src/test.cpp)

target_link_libraries(demo ${OXYGINE_CORE_LIBS})