cmake_minimum_required (VERSION 2.6)
project (TutorialResources)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(TutorialResources ../src/entry_point.cpp ../src/example.cpp  ../src/example.h )

if (WIN32) #disable console mode for VC++
	set_target_properties(TutorialResources PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)

target_link_libraries(TutorialResources ${OXYGINE_CORE_LIBS})