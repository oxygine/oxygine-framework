cmake_minimum_required (VERSION 2.6)
project (${PROJECT})

add_subdirectory(${OXYGINE} oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(${PROJECT} ${SRC} ${INCLUDE})

if (WIN32) #disable console mode for VC++
	set_target_properties(${PROJECT} PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)

target_link_libraries(${PROJECT} ${OXYGINE_CORE_LIBS})