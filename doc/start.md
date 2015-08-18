#How To Start

###Windows, MacOSX users
[Download](http://oxygine.org/download.php) Oxygine. I recommend to download **All-In-One** version with SDL. It includes everything to fast and easy start and has prebuilt SDL binaries.

Go to **oxygine-framework/examples** folder and open any of them.
There are examples:

* **HelloWorld** is starting Point.
* **Game** is tutorial example. It has 4 parts from beggining to advanced usage.
* **Demo** has tests and demonstation Oxygine functionality. 
* **DemoBox2D** is example with integrated Box2D.
* **Match3** is match3 mechanics demonstration implemented on Oxygine.
 
Each example has configuration folders for different platforms/OS:

* **proj.win32** - solutions for VisualStudio
* **proj.ios** - iOS xcode project
* **proj.macosx** - MacOSX xcode project
* **proj.marmalade** - Marmalade project
* *proj.android* - build folder for Android SDK/NDK
* *proj.cmake* - CMake configuration folder (Windows | Linux)
* *proj.emscripten* - Emscripten configuration folder

> if you have any troubles or if you need to build for android, cmake, emscripten check **oxygine-framework/readme** folder

###Advanced users 

If you want to setup and build everything yourself then [download](http://oxygine.org/download.php) oxygine framework only.
   
Go to **oxygine-framework/readme** folder and follow instructions **readme.txt**

###Marmalade SDK
If you are *MarmaladeSDK* user then follow *Advanced users* in instructions.


##Next Steps
Oxygine has some tools written in Python and you need to install them for advanced usage.

* [Download](https://www.python.org/downloads/) and install Python version 2.7.*
* [Download](http://www.pythonware.com/products/pil/) and install PIL for Python

[see details](https://bitbucket.org/oxygine/oxygine-framework/src/tip/tools/readme.txt?at=tip)

##Generate own project 

You could generate own project outside of oxygine-framework folder by **gen_template.py** tool. It is located in
  
	oxygine-framework/tools/

![](img/gen_template.png)

Run **gen_template.py** script with Python, example:

	python gen_template.py MyProjectName -t win32 -d path/to/MyProject/

It would create empty solution *MyProject* for Visual Studio in the folder *path/to/MyProject*


Other example:

	python gen_template.py MyProjectName -t all --hello -d path/to/MyProject/

It would create full copy of examples/HelloWorld including src, data and proj.* folders.


Next step is add *entry_point.cpp* (copy it from any example) to your project and implement basic functions (example_init, example_update and so on). Also, remember to copy the data folder as well if you are using a ready-made example code to start your project.

see for more info: 
	
	python gen_template.py --help

