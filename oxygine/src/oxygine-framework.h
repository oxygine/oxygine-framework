#pragma once
#ifndef OXYGINE_FRAMEWORK
#define OXYGINE_FRAMEWORK
//oxygine single include

//
#include "oxygine/oxygine-include.h"
#include "oxygine/actor/Actor.h"
#include "oxygine/AnimationFrame.h"
#include "oxygine/actor/Box9Sprite.h"
#include "oxygine/actor/Button.h"
#include "oxygine/actor/ClipRectActor.h"
#include "oxygine/Clock.h"
#include "oxygine/actor/ColorRectSprite.h"
#include "oxygine/actor/DebugActor.h"
#include "oxygine/Draggable.h"
#include "oxygine/Event.h"
#include "oxygine/EventDispatcher.h"
#include "oxygine/HttpRequestTask.h"
#include "oxygine/Font.h"
#include "oxygine/initActor.h"
#include "oxygine/Input.h"
#include "oxygine/InputText.h"
#include "oxygine/Image.h"
#include "oxygine/MemoryTexture.h"
#include "oxygine/ThreadLoader.h"
#include "oxygine/PointerState.h"
#include "oxygine/actor/ProgressBar.h"
#include "oxygine/RenderState.h"
#include "oxygine/actor/Stage.h"
#include "oxygine/actor/SlidingActor.h"
#include "oxygine/actor/Sprite.h"
#include "oxygine/actor/MaskedSprite.h"
#include "oxygine/actor/TextField.h"
#include "oxygine/TextStyle.h"
#include "oxygine/tween/Tween.h"
#include "oxygine/tween/TweenQueue.h"
#include "oxygine/TouchEvent.h"
#include "oxygine/KeyEvent.h"
#include "oxygine/UpdateState.h"
#include "oxygine/VisualStyle.h"
#include "oxygine/Serialize.h"
#include "oxygine/STDRenderer.h"
#include "oxygine/actor/WebImage.h"
#include "oxygine/key.h"
#include "oxygine/actor/Polygon.h"
#include "oxygine/tween/TweenOutline.h"
#include "oxygine/tween/TweenGlow.h"
#include "oxygine/tween/TweenAlphaFade.h"
#include "oxygine/MaterialX.h"
#include "oxygine/MaterialCache.h"

//closure/
#include "oxygine/closure/closure.h"
//#include "oxygine/closure/closure_impl.h"

//core/
#include "oxygine/core/file.h"
#include "oxygine/core/ImageData.h"
#include "oxygine/core/ImageDataOperations.h"
#include "oxygine/core/intrusive_ptr.h"
#include "oxygine/core/log.h"
#include "oxygine/core/Mem2Native.h"
#include "oxygine/core/Mutex.h"
#include "oxygine/core/NativeTexture.h"
#include "oxygine/core/Object.h"
#include "oxygine/core/oxygine.h"
#include "oxygine/core/ox_debug.h"
#include "oxygine/core/pixel.h"
#include "oxygine/core/ref_counter.h"
#include "oxygine/core/Renderer.h"
#include "oxygine/core/ShaderProgram.h"
#include "oxygine/core/Texture.h"
#include "oxygine/core/vertex.h"
#include "oxygine/core/VertexDeclaration.h"
#include "oxygine/core/VideoDriver.h"
#include "oxygine/core/STDFileSystem.h"
#include "oxygine/core/UberShaderProgram.h"

//core/gl/
#include "oxygine/core/gl/NativeTextureGLES.h"
#include "oxygine/core/gl/oxgl.h"
#include "oxygine/core/gl/VertexDeclarationGL.h"
#include "oxygine/core/gl/VideoDriverGLES20.h"
#include "oxygine/core/gl/ShaderProgramGL.h"

//dev_tools/
//#include "oxygine/dev_tools/DeveloperMenu.h"
//#include "oxygine/dev_tools/TexturesInspector.h"
//#include "oxygine/dev_tools/TreeInspector.h"
//#include "oxygine/dev_tools/TreeInspectorLine.h"
//#include "oxygine/dev_tools/TreeInspectorPage.h"
//#include "oxygine/dev_tools/TreeInspectorPreview.h"

//math/
#include "oxygine/math/AffineTransform.h"
#include "oxygine/math/Color.h"
#include "oxygine/math/Matrix.h"
#include "oxygine/math/Rect.h"
#include "oxygine/math/ScalarMath.h"
#include "oxygine/math/Vector2.h"
#include "oxygine/math/Vector3.h"
#include "oxygine/math/Vector4.h"

//pugixml/
//#include "oxygine/pugixml/pugiconfig.hpp"
#include "oxygine/pugixml/pugixml.hpp"

//res/
#include "oxygine/res/CreateResourceContext.h"
#include "oxygine/res/ResAnim.h"
#include "oxygine/res/ResAtlas.h"
#include "oxygine/res/ResBuffer.h"
#include "oxygine/res/ResFont.h"
#include "oxygine/res/ResFontBM.h"
#include "oxygine/res/Resource.h"
#include "oxygine/res/Resources.h"
#include "oxygine/res/ResStarlingAtlas.h"
#include "oxygine/res/SingleResAnim.h"

//text_utils/
#include "oxygine/text_utils/Aligner.h"
#include "oxygine/text_utils/Node.h"
#include "oxygine/text_utils/TextBuilder.h"

//tree_inspector/

//tree_inspector/internal/

//utils/
#include "oxygine/utils/AtlasBuilder.h"
#include "oxygine/utils/ImageUtils.h"
#include "oxygine/utils/intrusive_list.h"
#include "oxygine/utils/stringUtils.h"

//winnie_alloc/
//#include "oxygine/winnie_alloc/system_alloc.h"
//#include "oxygine/winnie_alloc/winnie_alloc.h"
//#include "oxygine/winnie_alloc/winnie_alloc_config.h"
#endif
