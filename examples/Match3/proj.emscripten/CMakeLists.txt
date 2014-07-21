cmake_minimum_required (VERSION 2.6)
project (Match3)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(Match3 ../src/entry_point.cpp ../src/example.cpp ../src/gameframe.cpp ../src/jewels.cpp ../src/shared.cpp  ../src/example.h ../src/gameframe.h ../src/jewels.h ../src/shared.h )

set_target_properties(Match3
    PROPERTIES 
    LINK_FLAGS "-s WARN_ON_UNDEFINED_SYMBOLS=1 -s -Wno-warn-absolute-paths -s TOTAL_MEMORY=64435456")

target_link_libraries(Match3 ${OXYGINE_CORE_LIBS})
em_link_pre_js(Match3 ${CMAKE_CURRENT_SOURCE_DIR}/data.js)