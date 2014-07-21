cmake_minimum_required (VERSION 2.6)
project (${PROJECT})

add_subdirectory(${OXYGINE} oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(${PROJECT} ${SRC} ${INCLUDE})

set_target_properties(${PROJECT}
    PROPERTIES 
    LINK_FLAGS "-s WARN_ON_UNDEFINED_SYMBOLS=1 -s -Wno-warn-absolute-paths -s TOTAL_MEMORY=64435456")

target_link_libraries(${PROJECT} ${OXYGINE_CORE_LIBS})
em_link_pre_js(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/data.js)