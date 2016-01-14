# BUILD AND RUN   
See **oxygine-framework/readme/** folder. It has insructions how to build and run oxygine on different platforms.

# Online documentation available at
https://bitbucket.org/oxygine/oxygine-framework/wiki/Home


# Oxygine is C++ engine for 2D mobile games. 
One more shell around OpenGL/D3D API you would say? No, this is a set of classes and tools to create a game according to your requirements.

In the basis of the engine there is a scene graph, that is similar to Flash one. To be short, You can call this as Flash for C++, but more comfortable and way faster. Initially it was developed for mobile platforms (iOS, Android), but can be also used for PC games.

Source code is available under MIT license (use for free anywhere).

##SDK includes:
- Source code, libraries
- Necessary tools, documentation
- Examples
- Demo for testing and demonstration of base features

##Supported platforms:
- Android
- iOS
- Windows
- MacOSX
- Emscripten (web)

##Examples of code:

```cpp
//create sprite and display it
spSprite sprite = new Sprite();
sprite->setResAnim(gameResources.getResAnim("anim"));
sprite->attachTo(getStage());
sprite->setScale(0.5f);

//run animation tween
sprite->addTween(TweenAnim(gameResources.getResAnim("anim2")), duration, loops);

//run position tween
sprite->addTween(Sprite::TweenPosition(x, y), duration, loops);

//handle events
sprite->addEventListener(TouchEvent::CLICK, clickHandler);
```		     

##Features:
- Programming language is C++.
- For rendering OpenGL(ES) 2.0 is used. Custom shaders supported.
- Compressed textures supported.
- Component based system, simple and predictable OOP API. Possibility to inherit from classes and overload their behavior.
- No global mega managers of everything, that dictates requirements to the way you write your code.
- Effective and flexible work with resources, very close to your game.
- Components of the engine have weak links between each other. Can be used together, separately or not at all.
- Fast creation and deletion of objects using memory pools for all engine objects.
- SceneGraph is somewhat similar to Flash ([diagrams with inheritance models](https://dl.dropbox.com/u/12679384/oxygine/actor_inheritance.png)). It is built on smart pointers (analogue boost:intrusive_ptr). No need to hassle with memory control, memory leaks. 
- Debug Tools ([screenshot](https://dl.dropboxusercontent.com/u/12679384/oxygine/debug_tools.gif)). Tree Inspector is “Visual profiler” for Scene Graph. Right in the game you can view the graph and different branches to analyze and errors search. “Textures Inspector” shows you currently loaded textures into memory ([screenshot](https://dl.dropbox.com/u/12679384/oxygine/textures_inspector.png))
- Render to texture.
- Resources description is stored in xml file. It has multiple different parameters: you can add your own resources types, format can be set, in which you will store the texture(atlas) in memory, number of columns and rows for animation, etc. Any resource can have user-data attributes.
- Tweens for frame by frame animation, color, position, rotation, etc. Your own can be easily created.
- Set of basic UI components commonly used in games: Button, Sprite, ProgressBar and others ([image](https://dl.dropboxusercontent.com/u/12679384/oxygine/actor_inheritance.png))
Command-line tool on Python to process your resources: Build you resources from XML files into atlases, determining the optimal size of atlases. Resize assets from high-res to necessary for your game. Does additional work to optimize work with resources in the game. Compress automatically to ETC1 or PVRTC textures format.
- Well-designed handling of events from mouse and multi-touch (for example you can press two buttons with two fingers at the same time without collisions) based on AS3 Flash model.
- It is possible to write code and dialogs in blocking style (concurency programming/coroutines).
- Accurate rendering of text with line wraps, vertical and horizontal alignment support, localization, utf-8. Base HTML tags support ([screenshot](https://dl.dropboxusercontent.com/u/12679384/oxygine/text_align.gif)).
- If you want to add multiple sets of assets for different game resolutions it can be done on the engine level by writing just a few lines of code. You won’t need to adjust coordinates manually for different resolution. It is enough just to keep initial art in high-res.
- Masking ([screenshot](https://dl.dropboxusercontent.com/u/12679384/oxygine/masking.gif), [screenshot](https://dl.dropboxusercontent.com/u/12679384/oxygine/mask.png))
- Atlas assembling on the fly during the loading, if you didn't make it before.
- Multi-threaded resources loading.
- It is possible to partially unload from memory “heavy” resources, like atlases. The resouce handle to work with atlas will be still valid (as well as his size) and will even have texture point, but the texture itself will be empty. This is very important when you have strict memory limitations: for example if you created all UI with all resources loaded, but there is no need to display them.
- Fast automatic batching.
- Bitmap fonts based on [BMFont](http://www.angelcode.com/products/bmfont/) and FreeType. Possibility to add your own font type.

You could build it on top of the Marmalade, SDL2 or adapt it for your platform. Oxygine could be build with Emscripten for internet browser as well.

And this is not all. There are a lot of features, but it is easy to use them, framework is not a monster and very intuitively clear. Viewing of couple examples would be enough to understand how it works.


##Contacts
You could find more information on [oxygine.org](http://oxygine.org)
