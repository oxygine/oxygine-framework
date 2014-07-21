cmake_minimum_required (VERSION 2.6)
project (HelloWorld)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(HelloWorld ../src/entry_point.cpp ../src/example.cpp  ../src/example.h )

set_target_properties(HelloWorld
    PROPERTIES 
    LINK_FLAGS "-s WARN_ON_UNDEFINED_SYMBOLS=1 -s -Wno-warn-absolute-paths -s TOTAL_MEMORY=64435456")

target_link_libraries(HelloWorld ${OXYGINE_CORE_LIBS})
em_link_pre_js(HelloWorld ${CMAKE_CURRENT_SOURCE_DIR}/data.js)