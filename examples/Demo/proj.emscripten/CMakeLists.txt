cmake_minimum_required (VERSION 2.6)
project (Demo)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

SET(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths")

add_executable(Demo ../src/entry_point.cpp ../src/example.cpp ../src/test.cpp  ../src/TestBox9Sprite.h ../src/TestClipRect.h ../src/TestDrag.h ../src/TestHttp.h ../src/TestInputText.h ../src/TestManageRes.h ../src/TestMask.h ../src/TestPerf.h ../src/TestPolygon.h ../src/TestProgressBar.h ../src/TestRender2Texture.h ../src/TestSliding.h ../src/TestTexel2Pixel.h ../src/TestText.h ../src/TestTextureFormat.h ../src/TestTweens.h ../src/TestUserShader.h ../src/TestUserShader2.h ../src/example.h ../src/test.h )

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	SET(linkFlags "-g4 ")
else() # Either MinSizeRel, RelWithDebInfo or Release, all which run with optimizations enabled.
	SET(linkFlags "-O2 ")
endif()

set(linkFlags "${linkFlags} -s NO_EXIT_RUNTIME=1 -s FULL_ES2=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -Wno-warn-absolute-paths -s TOTAL_MEMORY=50331648")

set_target_properties(Demo PROPERTIES LINK_FLAGS "${linkFlags}")

target_link_libraries(Demo ${OXYGINE_CORE_LIBS})
em_link_pre_js(Demo  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)