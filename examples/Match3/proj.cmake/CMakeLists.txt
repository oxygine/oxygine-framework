cmake_minimum_required (VERSION 2.6)
project (Match3)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(Match3 ../src/Board.cpp ../src/Jewel.cpp ../src/entry_point.cpp ../src/example.cpp ../src/shared.cpp  ../src/Board.h ../src/Jewel.h ../src/example.h ../src/shared.h )

if (WIN32) #disable console mode for VC++
	set_target_properties(Match3 PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)

target_link_libraries(Match3 ${OXYGINE_CORE_LIBS})