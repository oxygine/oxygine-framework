cmake_minimum_required (VERSION 2.6)
project (GamePart1)

add_subdirectory(../../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(GamePart1 ../src/Game.cpp ../src/Joystick.cpp ../src/Player.cpp ../src/Unit.cpp ../src/entry_point.cpp ../src/example.cpp ../src/res.cpp  ../src/Game.h ../src/Joystick.h ../src/Player.h ../src/Unit.h ../src/example.h ../src/res.h )

set_target_properties(GamePart1
    PROPERTIES 
    LINK_FLAGS "-s FULL_ES2=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 -s -Wno-warn-absolute-paths -s TOTAL_MEMORY=64435456")

target_link_libraries(GamePart1 ${OXYGINE_CORE_LIBS})
em_link_pre_js(GamePart1 ${CMAKE_CURRENT_SOURCE_DIR}/data.js)