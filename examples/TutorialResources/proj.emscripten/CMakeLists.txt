cmake_minimum_required (VERSION 2.6)
project (TutorialResources)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(TutorialResources ../src/entry_point.cpp ../src/example.cpp  ../src/example.h )

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	SET(linkFlags "-g4 ")
else() # Either MinSizeRel, RelWithDebInfo or Release, all which run with optimizations enabled.
	SET(linkFlags "-O2 ")
endif()

set(linkFlags "${linkFlags} -s NO_EXIT_RUNTIME=1 -s FULL_ES2=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -Wno-warn-absolute-paths -s TOTAL_MEMORY=33554432")

set_target_properties(TutorialResources PROPERTIES LINK_FLAGS "${linkFlags}")

target_link_libraries(TutorialResources ${OXYGINE_CORE_LIBS})
em_link_pre_js(TutorialResources  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)