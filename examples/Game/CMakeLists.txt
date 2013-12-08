cmake_minimum_required (VERSION 2.6)
project (Game)

file(GLOB_RECURSE PARTSRC
		part1/src/*.cpp
		part1/src/*.h)

add_executable(GamePart1
	${PARTSRC})

target_link_libraries(GamePart1 ${OXYGINE_CORE_LIBS})


file(GLOB_RECURSE PARTSRC
		part2/src/*.cpp
		part2/src/*.h)

add_executable(GamePart2
	${PARTSRC})

target_link_libraries(GamePart2 ${OXYGINE_CORE_LIBS})
