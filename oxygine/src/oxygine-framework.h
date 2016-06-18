#pragma once
#ifndef OXYGINE_FRAMEWORK
#define OXYGINE_FRAMEWORK
//oxygine single include

//
#include "oxygine_include.h"
#include "Actor.h"
#include "AnimationFrame.h"
#include "blocking.h"
#include "Box9Sprite.h"
#include "Button.h"
#include "ClipRectActor.h"
#include "Clock.h"
#include "ColorRectSprite.h"
#include "DebugActor.h"
#include "Draggable.h"
#include "Event.h"
#include "EventDispatcher.h"
#include "HttpRequestTask.h"
#include "Font.h"
#include "initActor.h"
#include "Input.h"
#include "InputText.h"
#include "MemoryTexture.h"
#include "ThreadLoader.h"
#include "PointerState.h"
#include "ProgressBar.h"
#include "RenderState.h"
#include "Stage.h"
#include "SlidingActor.h"
#include "Sprite.h"
#include "MaskedSprite.h"
#include "TextField.h"
#include "TextStyle.h"
#include "Tween.h"
#include "TweenQueue.h"
#include "TouchEvent.h"
#include "KeyEvent.h"
#include "UpdateState.h"
#include "VisualStyle.h"
#include "Serialize.h"
#include "STDRenderer.h"
#include "WebImage.h"

//closure/
#include "closure/closure.h"
//#include "closure/closure_impl.h"

//core/
#include "core/coroutines.h"
#include "core/file.h"
#include "core/ImageData.h"
#include "core/ImageDataOperations.h"
#include "core/intrusive_ptr.h"
#include "core/log.h"
#include "core/Mem2Native.h"
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
#include "core/STDFileSystem.h"


//core/gl/
//#include "core/gl/NativeTextureGLES.h"
//#include "core/gl/oxgl.h"
//#include "core/gl/VertexDeclarationGL.h"
//#include "core/gl/VideoDriverGL.h"
//#include "core/gl/VideoDriverGLES20.h"

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
#include "math/Matrix.h"
#include "math/Rect.h"
#include "math/ScalarMath.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

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
