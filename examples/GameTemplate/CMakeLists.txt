cmake_minimum_required (VERSION 2.6)
project (GAMETEMPLATE)


file(GLOB SRC
		src/*.cpp
		src/*.h)

add_executable(gametemplate ${SRC})

target_link_libraries(gametemplate ${OXYGINE_CORE_LIBS})