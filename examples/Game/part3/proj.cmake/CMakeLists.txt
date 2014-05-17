cmake_minimum_required (VERSION 2.6)
project (GamePart3)

add_subdirectory(../../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

add_executable(GamePart3 ../src/Enemy.cpp ../src/entry_point.cpp ../src/example.cpp ../src/Game.cpp ../src/GameScene.cpp ../src/Joystick.cpp ../src/MainMenuScene.cpp ../src/MyButton.cpp ../src/Player.cpp ../src/res.cpp ../src/Rocket.cpp ../src/Scene.cpp ../src/Unit.cpp  ../src/Enemy.h ../src/example.h ../src/Game.h ../src/GameScene.h ../src/Joystick.h ../src/MainMenuScene.h ../src/MyButton.h ../src/Player.h ../src/res.h ../src/Rocket.h ../src/Scene.h ../src/Unit.h )

target_link_libraries(GamePart3 ${OXYGINE_CORE_LIBS})