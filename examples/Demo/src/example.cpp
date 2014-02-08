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
#include "TestMask.h"
#include "TestInputText.h"
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


class TestActor: public Test
{
public:		

	TestActor()
	{	
		_x = 90;//getRoot()->getWidth()/2.0f;
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
		addButton("mask", "Mask");		
		addButton("inputtext", "Input Text");		
		
	}

	void clicked(string id)
	{
		setVisible(false);

		if (id == "perf")
		{
			getRoot()->addChild(new PerfTest);
		}
		if (id == "tweens")
		{
			getRoot()->addChild(new TweensTest);
		}
		if (id == "drag")
		{
			getRoot()->addChild(new DragTest);
		}
		if (id == "drag2")
		{
			getRoot()->addChild(new Drag2Test);
		}
		if (id == "manage_res")
		{
			getRoot()->addChild(new ManageResTest);
		}
		if (id == "r2t")
		{
			getRoot()->addChild(new TestRender2Texture);
		}
		if (id == "text")
		{
			getRoot()->addChild(new TestText);
		}

		if (id == "progress_bar")
		{
			getRoot()->addChild(new TestProgressBar);
		}

		if (id == "texture_format")
		{
			getRoot()->addChild(new TestTextureFormat);
		}

		if (id == "sliding")
		{
			getRoot()->addChild(new TestSliding);
		}

		if (id == "t2p")
		{
			getRoot()->addChild(new TestTexel2Pixel);
		}

		if (id == "box9sprite")
		{
			getRoot()->addChild(new TestBox9Sprite);
		}

		if (id == "cliprect")
		{
			getRoot()->addChild(new TestClipRect);
		}

		if (id == "usershader")
		{
			getRoot()->addChild(new TestUserShader);
		}

		if (id == "mask")
		{
			getRoot()->addChild(new TestMask);
		}

		if (id == "inputtext")
		{
			getRoot()->addChild(new TestInputText);
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
		arg_attachTo = getRoot(),
		arg_priority = 10,
		arg_alpha = 128
		);

	sp->setX(getRoot()->getWidth() - sp->getWidth());
	sp->setY(getRoot()->getHeight() - sp->getHeight());

	_tests = new TestActor;
	getRoot()->addChild(_tests);
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
}