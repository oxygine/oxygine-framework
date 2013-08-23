cmake_minimum_required (VERSION 2.6)
project (DEMO)

#add oxygine-framework/oxygine as child project
add_subdirectory(../../ oxygine)

add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(SRC src/entry_point.cpp
		src/example.cpp
		src/test.cpp)

add_executable(demo ${SRC})
target_link_libraries(demo ${OXYGINE_CORE_LIBS})