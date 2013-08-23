#pragma once
#ifndef OXYGINE_FRAMEWORK
#define OXYGINE_FRAMEWORK
//oxygine single include

//
#include "Actor.h"
#include "AnimationFrame.h"
#include "Blocking.h"
#include "Box9Sprite.h"
#include "Button.h"
#include "ClipRectActor.h"
#include "Clock.h"
#include "ColorRectSprite.h"
#include "DebugActor.h"
#include "Draggable.h"
#include "Event.h"
#include "EventDispatcher.h"
#include "Font.h"
#include "InitActor.h"
#include "Input.h"
#include "MemoryTexture.h"
#include "Multithreading.h"
#include "Options.h"
#include "PointerState.h"
#include "ProgressBar.h"
#include "RenderState.h"
#include "RootActor.h"
#include "SlidingActor.h"
#include "Sprite.h"
#include "TextActor.h"
#include "TextStyle.h"
#include "Tweener.h"
#include "UpdateState.h"
#include "VisualStyle.h"

//closure/
#include "closure/closure.h"
//#include "closure/closure_impl.h"

//core/
#include "core/coroutines.h"
#include "core/files_io.h"
#include "core/ImageData.h"
#include "core/ImageDataOperations.h"
#include "core/intrusive_ptr.h"
#include "core/logging.h"
#include "core/Mem2Native.h"
#include "core/memory_pool.h"
#include "core/Mutex.h"
#include "core/NativeTexture.h"
#include "core/Object.h"
#include "core/oxygine.h"
#include "core/ox_debug.h"
#include "core/pixel.h"
#include "core/ref_counter.h"
#include "core/Renderer.h"
#include "core/ShaderProgram.h"
#include "core/Texture.h"
#include "core/vertex.h"
#include "core/VertexDeclaration.h"
#include "core/VideoDriver.h"

//core/gl/
//#include "core/gl/NativeTextureGLES.h"
//#include "core/gl/oxgl.h"
//#include "core/gl/VertexDeclarationGL.h"
//#include "core/gl/VideoDriverGL.h"
//#include "core/gl/VideoDriverGLES11.h"
//#include "core/gl/VideoDriverGLES20.h"

//core/stage3d/
/*
#include "core/stage3d/NativeTextureStage3D.h"
#include "core/stage3d/VideoDriverStage3D.h"
*/

//dev_tools/
//#include "dev_tools/DeveloperMenu.h"
//#include "dev_tools/TexturesInspector.h"
//#include "dev_tools/TreeInspector.h"
//#include "dev_tools/TreeInspectorLine.h"
//#include "dev_tools/TreeInspectorPage.h"
//#include "dev_tools/TreeInspectorPreview.h"

//math/
#include "math/AffineTransform.h"
#include "math/Color.h"
#include "math/matrix.h"
#include "math/Rect.h"
#include "math/ScalarMath.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"

//pugixml/
//#include "pugixml/pugiconfig.hpp"
#include "pugixml/pugixml.hpp"

//res/
#include "res/CreateResourceContext.h"
#include "res/ResAnim.h"
#include "res/ResAtlas.h"
#include "res/ResBuffer.h"
#include "res/ResFont.h"
#include "res/ResFontBM.h"
#include "res/Resource.h"
#include "res/Resources.h"
#include "res/ResStarlingAtlas.h"

//text_utils/
#include "text_utils/Aligner.h"
#include "text_utils/Node.h"
#include "text_utils/TextBuilder.h"

//tree_inspector/

//tree_inspector/internal/

//utils/
#include "utils/AtlasTool.h"
#include "utils/ImageUtils.h"
#include "utils/intrusive_list.h"
#include "utils/stringUtils.h"

//winnie_alloc/
//#include "winnie_alloc/system_alloc.h"
//#include "winnie_alloc/winnie_alloc.h"
//#include "winnie_alloc/winnie_alloc_config.h"
#endif