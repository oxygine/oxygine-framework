#-------------------------------------------------
#
# Project created by QtCreator 2014-03-20T19:06:13
#
#-------------------------------------------------

QT       -= core gui

TARGET = oxygine-framework
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

ROOT = ../../../../
SRC = ../../../src
SOURCES += \
    $$SRC/Actor.cpp \ 
    $$SRC/AnimationFrame.cpp \ 
    $$SRC/blocking.cpp \ 
    $$SRC/Box9Sprite.cpp \ 
    $$SRC/Button.cpp \ 
    $$SRC/ClipRectActor.cpp \ 
    $$SRC/Clock.cpp \ 
    $$SRC/ColorRectSprite.cpp \ 
    $$SRC/DebugActor.cpp \ 
    $$SRC/Draggable.cpp \ 
    $$SRC/EventDispatcher.cpp \ 
    $$SRC/Font.cpp \ 
    $$SRC/Input.cpp \ 
    $$SRC/InputText.cpp \ 
    $$SRC/MaskedSprite.cpp \ 
    $$SRC/MemoryTexture.cpp \ 
    $$SRC/Multithreading.cpp \ 
    $$SRC/PointerState.cpp \ 
    $$SRC/ProgressBar.cpp \ 
    $$SRC/RootActor.cpp \ 
    $$SRC/SlidingActor.cpp \ 
    $$SRC/Sprite.cpp \ 
    $$SRC/TextActor.cpp \ 
    $$SRC/Tweener.cpp \ 
    $$SRC/VisualStyle.cpp \ 
    $$SRC/Serialize.cpp  

HEADERS +=  \
    $$SRC/Actor.h \ 
    $$SRC/AnimationFrame.h \ 
    $$SRC/blocking.h \ 
    $$SRC/Box9Sprite.h \ 
    $$SRC/Button.h \ 
    $$SRC/ClipRectActor.h \ 
    $$SRC/Clock.h \ 
    $$SRC/ColorRectSprite.h \ 
    $$SRC/DebugActor.h \ 
    $$SRC/Draggable.h \ 
    $$SRC/Event.h \ 
    $$SRC/EventDispatcher.h \ 
    $$SRC/Font.h \ 
    $$SRC/initActor.h \ 
    $$SRC/Input.h \ 
    $$SRC/InputText.h \ 
    $$SRC/MaskedSprite.h \ 
    $$SRC/MemoryTexture.h \ 
    $$SRC/Multithreading.h \ 
    $$SRC/oxygine-framework.h \ 
    $$SRC/oxygine_include.h \ 
    $$SRC/PointerState.h \ 
    $$SRC/ProgressBar.h \ 
    $$SRC/RenderState.h \ 
    $$SRC/RootActor.h \ 
    $$SRC/SlidingActor.h \ 
    $$SRC/Sprite.h \ 
    $$SRC/TextActor.h \ 
    $$SRC/TextStyle.h \ 
    $$SRC/Tweener.h \ 
    $$SRC/UpdateState.h \ 
    $$SRC/VisualStyle.h \ 
    $$SRC/Serialize.h


SOURCES += \
    $$SRC/core/coroutines.cpp \ 
    $$SRC/core/file.cpp \ 
    $$SRC/core/FileSystem.cpp \ 
    $$SRC/core/ImageData.cpp \ 
    $$SRC/core/ImageDataOperations.cpp \ 
    $$SRC/core/log.cpp \ 
    $$SRC/core/Mem2Native.cpp \ 
    $$SRC/core/memory_pool.cpp \ 
    $$SRC/core/Mutex.cpp \ 
    $$SRC/core/NativeTexture.cpp \ 
    $$SRC/core/Object.cpp \ 
    $$SRC/core/oxygine.cpp \ 
    $$SRC/core/Renderer.cpp \ 
    $$SRC/core/Restorable.cpp \ 
    $$SRC/core/STDFileSystem.cpp \ 
    $$SRC/core/system_data.cpp \ 
    $$SRC/core/Texture.cpp \ 
    $$SRC/core/ThreadMessages.cpp \ 
    $$SRC/core/UberShaderProgram.cpp \ 
    $$SRC/core/VideoDriver.cpp \ 
    $$SRC/core/ZipFileSystem.cpp \ 



HEADERS += \
$$SRC/core/coroutines.h \ 
    $$SRC/core/file.h \ 
    $$SRC/core/FileSystem.h \ 
    $$SRC/core/files_io.h \ 
    $$SRC/core/ImageData.h \ 
    $$SRC/core/ImageDataOperations.h \ 
    $$SRC/core/intrusive_ptr.h \ 
    $$SRC/core/log.h \ 
    $$SRC/core/logging.h \ 
    $$SRC/core/Mem2Native.h \ 
    $$SRC/core/memory_pool.h \ 
    $$SRC/core/Mutex.h \ 
    $$SRC/core/NativeTexture.h \ 
    $$SRC/core/Object.h \ 
    $$SRC/core/oxygine.h \ 
    $$SRC/core/ox_debug.h \ 
    $$SRC/core/pixel.h \ 
    $$SRC/core/ref_counter.h \ 
    $$SRC/core/Renderer.h \ 
    $$SRC/core/Restorable.h \ 
    $$SRC/core/ShaderProgram.h \ 
    $$SRC/core/STDFileSystem.h \ 
    $$SRC/core/system_data.h \ 
    $$SRC/core/Texture.h \ 
    $$SRC/core/ThreadMessages.h \ 
    $$SRC/core/UberShaderProgram.h \ 
    $$SRC/core/vertex.h \ 
    $$SRC/core/VertexDeclaration.h \ 
    $$SRC/core/VideoDriver.h \ 
    $$SRC/core/ZipFileSystem.h \ 


SOURCES += \
    $$SRC/dev_tools/DeveloperMenu.cpp \ 
    $$SRC/dev_tools/TexturesInspector.cpp \ 
    $$SRC/dev_tools/TreeInspector.cpp \ 
    $$SRC/dev_tools/TreeInspectorLine.cpp \ 
    $$SRC/dev_tools/TreeInspectorPage.cpp \ 
    $$SRC/dev_tools/TreeInspectorPreview.cpp \ 


HEADERS +=  \
    $$SRC/dev_tools/DeveloperMenu.h \ 
    $$SRC/dev_tools/TexturesInspector.h \ 
    $$SRC/dev_tools/TreeInspector.h \ 
    $$SRC/dev_tools/TreeInspectorLine.h \ 
    $$SRC/dev_tools/TreeInspectorPage.h \ 
    $$SRC/dev_tools/TreeInspectorPreview.h \ 


SOURCES += \
    $$SRC/minizip/ioapi.c \ 
    $$SRC/minizip/ioapi_mem.c \ 
    $$SRC/minizip/unzip.c \ 

HEADERS +=  \
    $$SRC/minizip/ioapi.h \ 
    $$SRC/minizip/ioapi_mem.h \ 
    $$SRC/minizip/unzip.h \ 

SOURCES += \
    $$SRC/pugixml/pugixml.cpp \ 

HEADERS +=  \
    $$SRC/pugixml/pugiconfig.hpp \ 
    $$SRC/pugixml/pugixml.hpp \ 

SOURCES += \
    $$SRC/res/CreateResourceContext.cpp \ 
    $$SRC/res/ResAnim.cpp \ 
    $$SRC/res/ResAtlas.cpp \ 
    $$SRC/res/ResBuffer.cpp \ 
    $$SRC/res/ResFont.cpp \ 
    $$SRC/res/ResFontBM.cpp \ 
    $$SRC/res/Resource.cpp \ 
    $$SRC/res/Resources.cpp \ 
    $$SRC/res/ResStarlingAtlas.cpp \ 

HEADERS += \
    $$SRC/res/CreateResourceContext.h \ 
    $$SRC/res/ResAnim.h \ 
    $$SRC/res/ResAtlas.h \ 
    $$SRC/res/ResBuffer.h \ 
    $$SRC/res/ResFont.h \ 
    $$SRC/res/ResFontBM.h \ 
    $$SRC/res/Resource.h \ 
    $$SRC/res/Resources.h \ 
    $$SRC/res/ResStarlingAtlas.h \ 

SOURCES += \
    $$SRC/text_utils/Aligner.cpp \ 
    $$SRC/text_utils/Node.cpp \ 
    $$SRC/text_utils/TextBuilder.cpp \ 

HEADERS += \
    $$SRC/text_utils/Aligner.h \ 
    $$SRC/text_utils/Node.h \ 
    $$SRC/text_utils/TextBuilder.h \ 

SOURCES += \
    $$SRC/utils/AtlasTool.cpp \ 
    $$SRC/utils/ImageUtils.cpp \ 
    $$SRC/utils/stringUtils.cpp \ 

HEADERS += \
    $$SRC/utils/AtlasTool.h \ 
    $$SRC/utils/ImageUtils.h \ 
    $$SRC/utils/intrusive_list.h \ 
    $$SRC/utils/stringUtils.h \ 

SOURCES += \
    $$SRC/winnie_alloc/system_alloc.cpp \ 
    $$SRC/winnie_alloc/winnie_alloc.cpp \ 

HEADERS += \
    $$SRC/winnie_alloc/system_alloc.h \ 
    $$SRC/winnie_alloc/winnie_alloc.h \ 
    $$SRC/winnie_alloc/winnie_alloc_config.h \ 

SOURCES += \
    $$SRC/core/gl/NativeTextureGLES.cpp \
    $$SRC/core/gl/oxgl.cpp \
    $$SRC/core/gl/VertexDeclarationGL.cpp \
    $$SRC/core/gl/ShaderProgramGL.cpp \
    $$SRC/core/gl/VideoDriverGL.cpp \
    $$SRC/core/gl/VideoDriverGLES20.cpp \

INCLUDEPATH += \
    $$SRC \
    $$SRC/../../../SDL/include/

debug
{
    DEFINES +=_DEBUG
    DEFINES += OXYGINE_QT
}
release
{
    DEFINES += OXYGINE_QT    
}

THIRD_PARTY = $$ROOT/oxygine/third_party/win32_mingw

INCLUDEPATH += $$THIRD_PARTY/libpng
INCLUDEPATH += $$THIRD_PARTY/zlib
INCLUDEPATH += $$THIRD_PARTY/pthreads

DEFINES += "OX_HAVE_LIBPNG=1"

unix {
    target.path = /usr/lib
    INSTALLS += target
}
