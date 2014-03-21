cmake_minimum_required (VERSION 2.6)
project (GamePart2)

add_subdirectory(../../../../..//oxygine-framework oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

add_executable(GamePart2 ../src/Enemy.cpp ../src/entry_point.cpp ../src/example.cpp ../src/Game.cpp ../src/Joystick.cpp ../src/Player.cpp ../src/res.cpp ../src/Rocket.cpp ../src/Unit.cpp  ../src/Enemy.h ../src/example.h ../src/Game.h ../src/Joystick.h ../src/Player.h ../src/res.h ../src/Rocket.h ../src/Unit.h )

target_link_libraries(GamePart2 ${OXYGINE_CORE_LIBS})