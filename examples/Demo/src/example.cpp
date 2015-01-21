#include "oxygine-framework.h"

#include "test.h"
#include "TestPerf.h"
#include "TestTweens.h"
#include "TestDrag.h"
#include "TestManageRes.h"
#include "TestRender2Texture.h"
#include "TestText.h"
#include "TestTextureFormat.h"
#include "TestTexel2Pixel.h"
#include "TestSliding.h"
#include "TestProgressBar.h"
#include "TestBox9Sprite.h"
#include "TestClipRect.h"
#include "TestUserShader.h"
#include "TestUserShader2.h"
#include "TestMask.h"
#include "TestPolygon.h"
#include "TestInputText.h"
#include "TestHttp.h"
#include "core/STDFileSystem.h"

#ifdef __S3E__
#include "s3eKeyboard.h"
#endif

using namespace oxygine;

spActor _tests;

//it is our resources
//in real project you would have more than one Resources declarations. It is important on mobile devices with limited memory and you would load/unload them
Resources resources;
Resources resourcesUI;

//extern spStage stage2;

class TestActor: public Test
{
public:		

	TestActor()
	{

		_x = 90;//getStage()->getWidth()/2.0f;
		_y = 80;


		addButton("tweens", "Tweens");
		addButton("text", "Text");
		addButton("progress_bar", "Progress Bar");
		addButton("drag", "Drag and Drop");
		addButton("drag2", "Drag and Drop2");
		addButton("perf", "Performance");
		addButton("manage_res", "Manage Resources");
		addButton("texture_format", "Textures Format");
		addButton("r2t", "Render to Texture");	
		addButton("t2p", "Texel to Pixel");
		addButton("sliding", "Sliding Actor");		
		addButton("box9sprite", "Box9 Sprite");		
		addButton("cliprect", "ClipRect Actor");		
		addButton("usershader", "User Shader");		
		addButton("usershader2", "User Shader2");
		addButton("mask", "Mask");		
		addButton("polygon", "Polygon");
		addButton("inputtext", "Input Text");		
		addButton("openbrowser", "Open Browser");
		addButton("http", "Http requests");
    }

	void showTest(spActor actor)
	{
		setVisible(false);
		//spStage stage = stage2;
		getStage()->addChild(actor);
	}

	void clicked(string id)
	{
		if (id == "perf")
		{
			showTest(new PerfTest);
		}
		if (id == "tweens")
		{
			showTest(new TweensTest);
		}
		if (id == "drag")
		{
			showTest(new DragTest);
		}
		if (id == "drag2")
		{
			showTest(new Drag2Test);
		}
		if (id == "manage_res")
		{
			showTest(new ManageResTest);
		}
		if (id == "r2t")
		{
			showTest(new TestRender2Texture);
		}
		if (id == "text")
		{
			showTest(new TestText);
		}

		if (id == "progress_bar")
		{
			showTest(new TestProgressBar);
		}

		if (id == "texture_format")
		{
			showTest(new TestTextureFormat);
		}

		if (id == "sliding")
		{
			showTest(new TestSliding);
		}

		if (id == "t2p")
		{
			showTest(new TestTexel2Pixel);
		}

		if (id == "box9sprite")
		{
			showTest(new TestBox9Sprite);
		}

		if (id == "cliprect")
		{
			showTest(new TestClipRect);
		}

		if (id == "usershader")
		{
			showTest(new TestUserShader);
		}

		if (id == "usershader2")
		{
			showTest(new TestUserShader2);
		}

		if (id == "mask")
		{
			showTest(new TestMask);
		}

		if (id == "polygon")
		{
			showTest(new TestPolygon);
		}

		if (id == "inputtext")
		{
			showTest(new TestInputText);
		}

		if (id == "openbrowser")
		{
			core::execute("http://oxygine.org/");
			setVisible(true);
		}

		if (id == "http")
		{
			showTest(new TestHttp);
		}
	}
};

void example_preinit()
{
	/**
	There are 2 modes of loading and blending/rendering sprites: normal and premultiplied alpha.
	You should set it before loading any assets. 
	Premultiplied mode is more advanced and faster than normal. In this mode RGB pixels of textures premultiplying to alpha when textures are loading and using blend_premultiply_alpha as default Sprites blend option.
	Default value is premultiplied = true
	http://blog.rarepebble.com/111/premultiplied-alpha-in-opengl/

	I set it to false to simplify shaders for UserShaderDemo
	*/

	Renderer::setPremultipliedAlphaRender(false);
}

file::STDFileSystem extfs(true);

void example_init()
{
	//mount additional file system with inner path "ext"
	//it would be used for searching path in data/ext
	extfs.setPath(file::fs().getFullPath("ext").c_str());
	file::mount(&extfs);

	//load xml file with resources definition
	resources.loadXML("xmls/res.xml");
	resourcesUI.loadXML("demo/res_ui.xml");
	resourcesUI.loadXML("demo/fonts.xml");

	spSprite sp = initActor(new Sprite, 
		arg_resAnim = resourcesUI.getResAnim("logo2"),
		arg_input = false,
		arg_attachTo = getStage(),
		arg_priority = 10,
		arg_alpha = 128
		);

	sp->setX(getStage()->getWidth() - sp->getWidth());
	sp->setY(getStage()->getHeight() - sp->getHeight());

	_tests = new TestActor;
	getStage()->addChild(_tests);
    
    //initialize http requests
    HttpRequestTask::init();
}

void example_update()
{

}

void example_destroy()
{
	_tests->detach();
	_tests = 0;
	resources.free();
	resourcesUI.free();
    HttpRequestTask::release();
}