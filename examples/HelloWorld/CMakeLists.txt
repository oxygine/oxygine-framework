cmake_minimum_required (VERSION 2.6)
project (HELLOWORLD)

add_executable(HelloWorld 
	src/entry_point.cpp
	src/example.h 
	src/example.cpp)

target_link_libraries(HelloWorld ${OXYGINE_CORE_LIBS})