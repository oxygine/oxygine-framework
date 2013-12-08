cmake_minimum_required (VERSION 2.6)
project (Game)

file(GLOB_RECURSE PART1SRC
		part1/src/*.cpp
		part1/src/*.h)

add_executable(GamePart1
	${PART1SRC})

target_link_libraries(GamePart1 ${OXYGINE_CORE_LIBS})
