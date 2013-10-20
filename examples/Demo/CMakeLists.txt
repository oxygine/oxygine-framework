cmake_minimum_required (VERSION 2.6)
project (DEMO)

add_executable(Demo 
	src/entry_point.cpp
	src/example.cpp
	src/example.h
	src/test.cpp
	src/test.h
	src/TestBox9Sprite.h
	src/TestClipRect.h
	src/TestComplexDrag.h
	src/TestDrag.h
	src/TestManageRes.h
	src/TestMask.h
	src/TestPerf.h
	src/TestProgressBar.h
	src/TestRender2Texture.h
	src/TestTexel2Pixel.h
	src/TestText.h
	src/TestTextureFormat.h
	src/TestTweens.h
	src/TestUserShader.h
	)

target_link_libraries(Demo ${OXYGINE_CORE_LIBS})
