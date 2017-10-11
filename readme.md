# BUILD AND RUN   
See **oxygine-framework/readme/** folder. It has instructions on how to build and run oxygine on different platforms.

# Wiki available at
https://github.com/oxygine/oxygine-framework/wiki


# Oxygine is a C++ engine for 2D mobile games. 
Yet another shell around OpenGL/D3D API you would say? No, this is a set of classes and tools to create a game according to your requirements.

At the basis of the engine there is a scene graph, that is similar to Flash. In short, You can call this as Flash for C++, but more comfortable and way faster. Initially it was developed for mobile platforms (iOS, Android), but can also be used for PC games.

Source code is available under MIT license (use for free anywhere).

## SDK includes:
- Source code, libraries
- Necessary tools, documentation
- Examples
- Demo for testing and demonstration of base features

## Supported platforms:
- Android
- iOS
- Windows
- MacOSX
- Emscripten (web)

## Examples of code:

```cpp
//create sprite and display it
spSprite sprite = new Sprite();
sprite->attachTo(getStage());

//run animation tween
sprite->addTween(Sprite::TweenAnim(gameResources.getResAnim("anim2")), duration, loops);

//run position tween
sprite->addTween(Sprite::TweenPosition(x, y), duration, loops);

//handle click event
sprite->addEventListener(TouchEvent::CLICK, [sprite](Event*){
    
    log::messageln("clicked to sprite!");
    //change sprite color
    sprite->setColor(Color::Gray);
});
```		     

## Features:
- Programming language is C++.
- For rendering OpenGL(ES) 2.0 is used. Custom shaders supported.
- Compressed textures supported.
- Component based system, simple and predictable OOP API. Possibility to inherit from classes and overload their behavior.
- No global mega managers of everything, that dictate the requirements and the way you write your code.
- Effective and flexible work with resources, very close to your game.
- Components of the engine have weak links between each other. Can be used together, separately or not at all.
- Fast creation and deletion of objects using memory pools for all engine objects.
- SceneGraph is somewhat similar to Flash ([diagrams with inheritance models](http://i.imgur.com/DO5WFUA.png)). It is built on smart pointers (analogue boost:intrusive_ptr). No need to hassle with memory control, memory leaks. 
- Debug Tools ([screenshot](http://i.imgur.com/77aGg82.gif)). Tree Inspector is “Visual profiler” for Scene Graph. Right in the game you can view the graph and different branches to analyze and errors search. “Textures Inspector” shows you currently loaded textures into memory ([screenshot](http://i.imgur.com/Znynji4.png))
- Render to texture.
- Resources description is stored in xml file. It has multiple different parameters: you can add your own resources types, format can be set, in which you will store the texture(atlas) in memory, number of columns and rows for animation, etc. Any resource can have user-data attributes.
- Tweens for frame-by-frame animation, color, position, rotation, etc. Easily create your own.
- A set of basic UI components commonly used in games: Button, Sprite, ProgressBar and others ([image](http://i.imgur.com/IUxKi7O.png))
Command-line tool on Python to process your resources: Build you resources from XML files into atlases, determining the optimal size of atlases. Resize assets from high-res to necessary for your game. Does additional work to optimize work with resources in the game. Compress automatically to ETC1 or PVRTC textures format.
- Well-designed handling of events from mouse and multi-touch (for example you can press two buttons with two fingers at the same time without collisions) based on AS3 Flash model.
- It is possible to write code and dialogs in blocking style (concurency programming/coroutines).
- Accurate rendering of text with line-wraps, vertical and horizontal alignment support, localization, utf-8. Base HTML tags support ([screenshot](http://i.imgur.com/x66UTR3.gif)).
- If you want to add multiple sets of assets for different game resolutions it can be done on the engine level by writing just a few lines of code. You won’t need to adjust coordinates manually for different resolution. It is enough just to keep initial art in high-res.
- Masking ([screenshot](http://i.imgur.com/dxSUVGX.gif), [screenshot](http://i.imgur.com/wAhd6BG.png))
- Atlas assembling on the fly during the loading, if you didn't provide it yourself.
- Multi-threaded resources loading.
- It is possible to partially unload “heavy” resources like atlases from memory. The resouce handle to work with atlas will be still valid (as well as its size) and will even have texture points, but the texture itself will be empty. This is very important when you have strict memory limitations: for example if you created the whole UI with all resources loaded, but there is no need to display them all at once.
- Fast automatic batching.
- Bitmap fonts based on [BMFont](http://www.angelcode.com/products/bmfont/) and FreeType. Possibility to add your own font type.

You could build it on top of Marmalade, SDL2 or adapt it for your platform. Oxygine can be built with Emscripten for internet browsers as well.

And this is not everything. There are a lot of easy-to-use features, this framework is not a monster and is clear and intuitive. Viewing a couple of examples is enough to understand how it works.


## Contacts
You can find more information at [oxygine.org](http://oxygine.org)
