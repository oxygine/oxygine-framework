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



file(GLOB_RECURSE PARTSRC
		part3/src/*.cpp
		part3/src/*.h)

add_executable(GamePart3
	${PARTSRC})

target_link_libraries(GamePart3 ${OXYGINE_CORE_LIBS})


file(GLOB_RECURSE PARTSRC
		part4/src/*.cpp
		part4/src/*.h)

add_executable(GamePart4
	${PARTSRC})

target_link_libraries(GamePart4 ${OXYGINE_CORE_LIBS})
