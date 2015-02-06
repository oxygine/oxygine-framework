cmake_minimum_required (VERSION 2.6)
project (GamePart4)

add_subdirectory(../../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(GamePart4 ../src/Enemy.cpp ../src/Game.cpp ../src/GameMenu.cpp ../src/GameScene.cpp ../src/Joystick.cpp ../src/MainMenuScene.cpp ../src/MyButton.cpp ../src/Player.cpp ../src/Rocket.cpp ../src/Scene.cpp ../src/Unit.cpp ../src/entry_point.cpp ../src/example.cpp ../src/res.cpp  ../src/Enemy.h ../src/Game.h ../src/GameMenu.h ../src/GameScene.h ../src/Joystick.h ../src/MainMenuScene.h ../src/MyButton.h ../src/Player.h ../src/Rocket.h ../src/Scene.h ../src/Unit.h ../src/example.h ../src/res.h )

target_link_libraries(GamePart4 ${OXYGINE_CORE_LIBS})