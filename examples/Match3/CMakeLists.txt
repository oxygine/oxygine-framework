cmake_minimum_required (VERSION 2.6)
project (MATCH3)

add_executable(Match3 
	src/entry_point.cpp
	src/example.h	
	src/example.cpp
	src/gameframe.h
	src/gameframe.cpp
	src/jewels.h
	src/jewels.cpp
	src/shared.h
	src/shared.cpp)

target_link_libraries(Match3 ${OXYGINE_CORE_LIBS})