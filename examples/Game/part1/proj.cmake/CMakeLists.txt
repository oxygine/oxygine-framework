cmake_minimum_required (VERSION 2.6)
project (GamePart1)

add_subdirectory(../../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(GamePart1 ../src/Game.cpp ../src/Joystick.cpp ../src/Player.cpp ../src/Unit.cpp ../src/entry_point.cpp ../src/example.cpp ../src/res.cpp  ../src/Game.h ../src/Joystick.h ../src/Player.h ../src/Unit.h ../src/example.h ../src/res.h )

if (WIN32) #disable console mode for VC++
	set_target_properties(GamePart1 PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)

target_link_libraries(GamePart1 ${OXYGINE_CORE_LIBS})
