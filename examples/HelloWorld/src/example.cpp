#include "oxygine-framework.h"
using namespace oxygine;

//it is our resources
//in real project you would have more than one Resources declarations. 
//It is important on mobile devices with limited memory and you would load/unload them
Resources gameResources;

//DECLARE_SMART is helper, it does forward declaration and declares intrusive_ptr typedef for your class
DECLARE_SMART(MainActor, spMainActor);

class MainActor: public Actor
{
public:
	spTextField _text;

	MainActor()
	{	
		//create Button actor
		spButton button = new Button();
		//add it as child to current actor
		addChild(button);

		//you will find 'button' resource definition in res.xml
		//button has 3 columns for each state: Normal, Pressed, Overed
		button->setResAnim(gameResources.getResAnim("button"));
		//centered button at screen	
		Vector2 pos = getRoot()->getSize()/2 - button->getSize()/2;
		button->setPosition(pos);
		button->setInputChildrenEnabled(false);

		//handle click to button
		EventCallback cb = CLOSURE(this, &MainActor::displayClicked);
		button->addEventListener(TouchEvent::CLICK, cb);

		//second part

		//create Actor with Text and it to button as child
		spTextField text = new TextField();
		text->attachTo(button);
		//centered in button
		text->setPosition(button->getSize()/2);

		//initialize text style
		//it would be centered and colored
		TextStyle style;
		style.font = gameResources.getResFont("main")->getFont();
		style.color = Color(72, 61, 139, 255);
		style.vAlign = TextStyle::VALIGN_MIDDLE;
		style.hAlign = TextStyle::HALIGN_CENTER;

		text->setStyle(style);
		text->setText("click\nme!");

		//we will change text later
		_text = text; 
	}

	void displayClicked(Event *event)
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

		//animation has 7 columns, check res.xml
		ResAnim *animation = gameResources.getResAnim("anim");

		//add animation tween to sprite
		//TweenAnim would change animation frames
		sprite->addTween(TweenAnim(animation), duration, loops);

		Vector2 destPos = getRoot()->getSize() - sprite->getSize();
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

		//and remove sprite from tree when tweenQueue is empty
		//if you don't hold any references to sprite it would be deleted automatically
		tweenQueue->setDetachActor(true);		
	}
};

void example_preinit()
{
}

void example_init()
{
	//load xml file with resources definition
	gameResources.loadXML("res.xml");


	//lets create our client code simple actor
	//prefix 'sp' here means it is intrusive Smart Pointer
	//it would be deleted automatically when you lost ref to it	
	spMainActor actor = new MainActor;
	//and add it to RootActor as child
	getRoot()->addChild(actor);
}

void example_update()
{
	
}

void example_destroy()
{
	gameResources.free();
}