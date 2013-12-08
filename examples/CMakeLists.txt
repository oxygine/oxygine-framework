cmake_minimum_required (VERSION 2.6)
project (EXAMPLES)

add_subdirectory(../ oxygine-framework)

add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})


add_subdirectory(Demo)
add_subdirectory(HelloWorld)
add_subdirectory(Match3)
add_subdirectory(Game)
add_subdirectory(DemoBox2D)
add_subdirectory(TutorialResources)