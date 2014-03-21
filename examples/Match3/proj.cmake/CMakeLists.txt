cmake_minimum_required (VERSION 2.6)
project (Match3)

add_subdirectory(../../../..//oxygine-framework oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

add_executable(Match3 ../src/entry_point.cpp ../src/example.cpp ../src/gameframe.cpp ../src/jewels.cpp ../src/shared.cpp  ../src/example.h ../src/gameframe.h ../src/jewels.h ../src/shared.h )

target_link_libraries(Match3 ${OXYGINE_CORE_LIBS})