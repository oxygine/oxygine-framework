cmake_minimum_required (VERSION 2.6)
project (TUTORIALRESOURCES)

add_executable(TutorialResources 
	src/entry_point.cpp
	src/example.h 
	src/example.cpp)

target_link_libraries(TutorialResources ${OXYGINE_CORE_LIBS})