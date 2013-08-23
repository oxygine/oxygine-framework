#include "test.h"
#include "oxygine-framework.h"

spTextActor createText(string txt)
{
	spTextActor text = new TextActor();

	TextStyle style;
	style.font = resourcesUI.getResFont("main")->getFont();
	style.color = Color(72, 61, 139, 255);
	//style.color = Color(255, 255, 255, 255);
	style.vAlign = TextStyle::VALIGN_MIDDLE;
	style.hAlign = TextStyle::HALIGN_CENTER;
	style.multiline = true;

	text->setStyle(style);
	text->setText(txt.c_str());

	return text;

}
spButton createButtonHelper(string txt, EventCallback cb)
{
	spButton button = new Button();
	button->setPriority(10);
	//button->setName(id);
	button->setResAnim(resourcesUI.getResAnim("button"));
	button->addEventListener(TouchEvent::CLICK, cb);


	//create Actor with Text and it to button as child
	spTextActor text = createText(txt);
	//text->setPosition(button->getSize()/2);
	text->setSize(button->getSize());
	text->attachTo(button);
	//text->setScale(0.8f);


	return button;
}


Test::Test()
{
	setSize(RootActor::instance->getSize());

	_x = getWidth() - 100;
	_y = 2;
	

	ui = new Actor;
	content = new Content;
	content->setSize(getSize());


	addChild(content);
	addChild(ui);

	if (_tests)
	{
		spButton button = createButtonHelper("back", CLOSURE(this, &Test::back));
		button->setY((float)getHeight() - button->getHeight());
		ui->addChild(button);
	}	
}


Test::~Test()
{
	int q=0;
}


void Test::addButton(string id, string txt)
{
	EventCallback cb = CLOSURE(this, &Test::_clicked);

	spButton button = createButtonHelper(txt, cb);
	button->setName(id);
	//add it as child to current actor
	ui->addChild(button);
	button->setAnchor(Vector2(0.5f, 0.0f));

	//center button at screen		
	button->setPosition(Vector2((float)_x, (float)_y));
	//button->setPosition(Vector2(150, _y));
	_y += button->getHeight() + 2.0f;
}

void Test::updateText(string id, string txt)
{
	spActor child = ui->getChild(id);
	if (!child)
		return;

	TextActor *t = safeCast<TextActor*>(child->getFirstChild().get());
	if (!t)
		return;
	t->setText(txt);
}

void Test::clicked(string id)
{

}

void Test::_clicked(Event *event)
{
	clicked(event->currentTarget->getName());
}


void Test::back(Event *event)
{
	detach();
	_tests->setVisible(true);
}


void Test::showPopup(string txt, int time)
{
	spSprite sprite = new Sprite();
	sprite->setPriority(10);
	sprite->setAnimFrame(resourcesUI.getResAnim("button"));
	sprite->setAlpha(0);

	spTweenQueue tq = new TweenQueue;
	tq->add(Actor::TweenAlpha(255), 300, 1, false, 0, Tween::ease_outExpo);
	tq->add(Actor::TweenAlpha(200), 300, time / 300, true);
	tq->add(Actor::TweenAlpha(0), 300);
	tq->setDetachActor(true);

	sprite->addTween(tq);
	sprite->attachTo(ui);
	sprite->setPosition(0.0f, getHeight() - 150.0f);
	

	spTextActor text = createText(txt);
	text->attachTo(sprite);
	text->setPosition(sprite->getSize()/2);
}