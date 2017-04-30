cmake_minimum_required (VERSION 2.6)
project (Demo)

add_subdirectory(../../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

add_executable(Demo ../src/Counter.cpp ../src/example.cpp ../src/main.cpp ../src/test.cpp  ../src/Counter.h ../src/TestAlphaHitTest.h ../src/TestBox9Sprite.h ../src/TestClipRect.h ../src/TestColorFont.h ../src/TestCounter.h ../src/TestDrag.h ../src/TestEdges.h ../src/TestHttp.h ../src/TestInputText.h ../src/TestManageRes.h ../src/TestMask.h ../src/TestPerf.h ../src/TestPolygon.h ../src/TestProgressBar.h ../src/TestRender2Texture.h ../src/TestSignedDistanceFont.h ../src/TestSliding.h ../src/TestTexel2Pixel.h ../src/TestText.h ../src/TestTextureFormat.h ../src/TestTouches.h ../src/TestTweenPostProcessing.h ../src/TestTweenShine.h ../src/TestTweenText.h ../src/TestTweens.h ../src/TestUserShader.h ../src/example.h ../src/test.h )
target_link_libraries(Demo ${OXYGINE_CORE_LIBS})



if (WIN32) #disable console mode for VC++
	set_target_properties(Demo PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)



if (EMSCRIPTEN)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648")
	em_link_pre_js(Demo  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)