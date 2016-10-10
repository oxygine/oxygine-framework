call emsdk activate

python ../../../..//tools/others/embed_folder_js.py -s ../data

mkdir build_emsc_release
cd build_emsc_release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="%EMSCRIPTEN%/cmake/Modules/Platform/emscripten.cmake" -G"Unix Makefiles" ..
make
cd ..