#include "oxygine-framework.h"
using namespace oxygine;

//it is our resources
//in real project you would have more than one Resources declarations. It is important on mobile devices with limited memory and you would load/unload them
Resources gameResources;

//DECLARE_SMART is helper, it does forward declaration and declares intrusive_ptr typedef for your class
DECLARE_SMART(MainActor, spMainActor);

class MainActor: public Actor
{
public:
	spTextActor _text;

	MainActor()
	{	
		//create Button actor
		spButton button = new Button();
		//add it as child to current actor
		addChild(button);

		//you will find 'button' resource definition in res.xml
		//button has 3 columns for each state: Normal, Pressed, Overed
		button->setResAnim(gameResources.getResAnim("button"));
		//center button at screen		
		//button->setPosition(Vector2(480, 320)/2 - button->getSize()/2);		
		button->setPosition(Vector2(480, 320)/2);		
		button->setAnchor(Vector2(0.5,0.5));
		button->setInputChildrenEnabled(false);

		//how to handle click
		//find out more about CLOSUREs here: http://dobrokot.nm.ru/cpp/CppMethodsCallback.html
		EventCallback cb = CLOSURE(this, &MainActor::clicked);
		button->addEventListener(TouchEvent::CLICK, cb);

		//second part

		//create Actor with Text and it to button as child
		spTextActor text = new TextActor();
		button->addChild(text);

		//initialize text style
		//it would be centered and colored
		TextStyle style;
		style.font = gameResources.getResFont("main")->getFont();
		style.color = Color(72, 61, 139, 255);
		style.vAlign = TextStyle::VALIGN_MIDDLE;
		style.hAlign = TextStyle::HALIGN_CENTER;

		text->setStyle(style);;		
		text->setText("click\nme!");

		//default size is (0,0)
		//set button size to text
		//align would work properly now
		text->setSize(button->getSize());	

		//_text is MainActor member
		//we will use it later
		_text = text; 
	}

	void clicked(Event *event)
	{
		//user clicked to button
		_text->setText("clicked!");

		//lets create and run sprite with animation
		runSprite();
	}

	void runSprite()
	{
		spSprite sprite = new Sprite();		
		addChild(sprite);

		int duration = 500;//500 ms 
		int loops = -1;//infinity loops

		//animation has 7 cols, check res.xml
		ResAnim *animation = gameResources.getResAnim("anim");

		//create animation tween and add it to sprite
		//TweenAnim would change animation frames
		spTween tween = createTween(TweenAnim(animation), duration, loops);
		sprite->addTween(tween);

		Vector2 destPos = RootActor::instance->getSize() - sprite->getSize();
		Vector2 srcPos = Vector2(0, destPos.y);
		//set sprite initial position
		sprite->setPosition(srcPos);		

		//add another tween: TweenQueue
		//TweenQueue is a collection of tweens
		spTweenQueue tweenQueue = new TweenQueue();
		tweenQueue->setDelay(1500);
		//first, move sprite to dest position
		tweenQueue->add(Sprite::TweenPosition(destPos), 1500, 1);
		//then fade it out smoothly
		tweenQueue->add(Sprite::TweenAlpha(0), 1000, 1);



		sprite->addTween(tweenQueue);

		//and remove sprite from actor when tweenQueue is empty
		//sprite would be deleted automatically
		tweenQueue->setDetachActor(true);		
	}
};

void example_init()
{
	//load xml file with resources definition
	gameResources.loadXML("res.xml");

	//lets create our client code simple actor
	//prefix 'sp' here means it is intrusive Smart Pointer
	//it would be deleted automatically when you lost ref to it	
	spMainActor actor = new MainActor;
	//and add it to RootActor as child
	RootActor::instance->addChild(actor);
}

void example_destroy()
{
	gameResources.free();
}