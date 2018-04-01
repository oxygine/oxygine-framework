cmake_minimum_required (VERSION 2.6)
project (GamePart1)

add_subdirectory(../../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

add_executable(GamePart1 ../src/Game.cpp ../src/Joystick.cpp ../src/Player.cpp ../src/Unit.cpp ../src/example.cpp ../src/main.cpp ../src/res.cpp  ../src/Game.h ../src/Joystick.h ../src/Player.h ../src/Unit.h ../src/example.h ../src/res.h )
target_link_libraries(GamePart1 ${OXYGINE_CORE_LIBS})



if (WIN32) #disable console mode for VC++
	set_target_properties(GamePart1 PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)



if (EMSCRIPTEN)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")
	em_link_pre_js(GamePart1  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)