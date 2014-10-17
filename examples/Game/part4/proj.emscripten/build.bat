python ../../../..//tools/others/embed_folder_js.py -s ../data

mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=%EMSCRIPTEN%/cmake/Modules/Platform/emscripten.cmake -G"Unix Makefiles" .. 
make
cd ..