cmake_minimum_required (VERSION 2.6)
project (GAMETEMPLATE)


file(GLOB SRC
	src/entry_point.cpp
	src/example.cpp
	src/example.h
	src/GameActor.cpp
	src/GameActor.h
	src/GameMenu.cpp
	src/GameMenu.h
	src/GameResult.cpp
	src/GameResult.h
	src/MainMenu.cpp
	src/MainMenu.h
	src/Modal.cpp
	src/Modal.h
	src/ModalActor.cpp
	src/ModalActor.h
	src/Options.cpp
	src/Options.h
	src/OptionsMenu.cpp
	src/OptionsMenu.h
	src/shared.cpp
	src/shared.h
	)

add_executable(GameTemplate ${SRC})

target_link_libraries(GameTemplate ${OXYGINE_CORE_LIBS})