cmake_minimum_required (VERSION 2.6)
project (GamePart1)

add_subdirectory(../../../../..//oxygine-framework oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

add_executable(GamePart1 ../src/entry_point.cpp ../src/example.cpp ../src/Game.cpp ../src/Joystick.cpp ../src/Player.cpp ../src/res.cpp ../src/Unit.cpp  ../src/example.h ../src/Game.h ../src/Joystick.h ../src/Player.h ../src/res.h ../src/Unit.h )

target_link_libraries(GamePart1 ${OXYGINE_CORE_LIBS})