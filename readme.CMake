1. read readme.SDL2


2. How to build and run Oxygine on Windows with CMake and Visual Studio (tested with VS2010 and VS2012). Steps:
=================================================================================================
- build SDL2
- copy SDL2.lib and SDL2main.lib to oxygine-framework/libs

- goto examples
- create folder "build"
- generate solution with cmake (from oxygine-framework/examples/build folder):
  cmake ..
- open solution. select any project for example Demo and set it "as startup project"
- set working directory for Demo project to: 
   oxygine-framework\examples\Demo\data\
- copy all required dlls to this folder
   oxygine-framework\examples\Demo\data\
   it is SDL2.dll and dlls from  oxygine-framework\oxygine\third_party\win32\libraries\
- ready! run!
=================================================================================================

3. How to build and run Oxygine on Linux with CMake
=================================================================================================
make and install SDL2
- goto examples
- create folder "build"
- make
- executable examples should be created
- copy required resources.
  For example if you want run Demo:
  - copy all files from example/Demo/data to examples/build/Demo
  - run ./Demo   
=================================================================================================
