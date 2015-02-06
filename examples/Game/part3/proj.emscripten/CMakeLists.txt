cmake_minimum_required (VERSION 2.6)
project (GamePart3)

add_subdirectory(../../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(GamePart3 ../src/Enemy.cpp ../src/Game.cpp ../src/GameScene.cpp ../src/Joystick.cpp ../src/MainMenuScene.cpp ../src/MyButton.cpp ../src/Player.cpp ../src/Rocket.cpp ../src/Scene.cpp ../src/Unit.cpp ../src/entry_point.cpp ../src/example.cpp ../src/res.cpp  ../src/Enemy.h ../src/Game.h ../src/GameScene.h ../src/Joystick.h ../src/MainMenuScene.h ../src/MyButton.h ../src/Player.h ../src/Rocket.h ../src/Scene.h ../src/Unit.h ../src/example.h ../src/res.h )

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	SET(linkFlags "-g4 ")
else() # Either MinSizeRel, RelWithDebInfo or Release, all which run with optimizations enabled.
	SET(linkFlags "-O2 ")
endif()

set(linkFlags "${linkFlags} -s NO_EXIT_RUNTIME=1 -s FULL_ES2=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -Wno-warn-absolute-paths -s TOTAL_MEMORY=50331648")

set_target_properties(GamePart3 PROPERTIES LINK_FLAGS "${linkFlags}")

target_link_libraries(GamePart3 ${OXYGINE_CORE_LIBS})
em_link_pre_js(GamePart3  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)