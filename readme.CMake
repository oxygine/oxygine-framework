1. read readme.SDL2

2. How to build and run Oxygine on Windows with CMake and Visual Studio (tested with VS2010 and VS2012). Steps:
=================================================================================================
- build SDL2. It could be done with cmake or prebuild VC++ solution. Find SDL2.lib, SDL2main.lib and SDL2.dll
- copy SDL2.lib and SDL2main.lib to oxygine-framework/libs

- select any example from examples folder. I chose Demo
- go to folder oxygine-framework/examples/demo/proj.cmake
- run: cmake
- open generated solution. 
- Select Demo project and set it "as startup project".
- set working directory for Demo project to: 
   oxygine-framework\examples\Demo\data\
- copy
   all required dlls from  oxygine-framework\oxygine\third_party\win32\dlls\
   and SDL2.dll 
   to oxygine-framework\examples\Demo\data\
   
- ready! run!
=================================================================================================

3. How to build and run Oxygine on Linux with CMake
=================================================================================================
make and install SDL2
- also should be installed packages (apt-get install):
  - libjpeg8-dev zlib1g-dev libpng12-dev
  - freeglut3 (needed only opengl)
- goto examples/Demo (or any other example)
- goto folder proj.cmake/
- run: cmake
- run: make
- binary executable  should be created
- copy required resources:
  - copy all files from example/Demo/data to examples/Demo/proj.cmake
  - run ./Demo   
=================================================================================================
