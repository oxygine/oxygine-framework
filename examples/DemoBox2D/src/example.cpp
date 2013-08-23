#include "oxygine-framework.h"
#include "Box2D/Box2D.h"

using namespace oxygine;

//it is our resources
//in real project you would have more than one Resources declarations. It is important on mobile devices with limited memory and you would load/unload them
Resources gameResources;

//DECLARE_SMART is helper, it does forward declaration and declares intrusive_ptr typedef for your class
DECLARE_SMART(MainActor, spMainActor);

const float SCALE = 100.0f;
b2Vec2 convert(const Vector2 &pos)
{
	return b2Vec2(pos.x / SCALE, pos.y / SCALE);
}

Vector2 convert(const b2Vec2 &pos)
{
	return Vector2(pos.x * SCALE, pos.y * SCALE);
}

class MainActor: public Actor
{
public:
	
	b2World *_world;

	MainActor():_world(0)
	{	
		setSize(RootActor::instance->getSize());


		spClock clock = new Clock();
		clock->setFixedStep(1000.0f / 60.0f);
		setClock(clock);

		addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::clicked));


		_world = new b2World(b2Vec2(0, 10), false);


		b2BodyDef groundBodyDef;
		groundBodyDef.position = convert(Vector2(0.0f, getHeight() + 0.1f * SCALE));
		b2Body* groundBody = _world->CreateBody(&groundBodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(getWidth()/SCALE, 0.1f);		
		groundBody->CreateFixture(&groundBox, 0.0f);
	}

	
	void doUpdate(const UpdateState &us)
	{
		_world->Step(us.dt / 1000.0f, 6, 2);

		b2Body *body = _world->GetBodyList();
		while(body)
		{
			Actor *actor = (Actor *)body->GetUserData();
			b2Body *next = body->GetNext();
			if (actor)
			{
				const b2Vec2& pos = body->GetPosition();
				actor->setPosition(convert(pos));
				actor->setRotation(body->GetAngle());

				if (actor->getX() < -50 || actor->getX() > getWidth() + 50)
				{
					body->SetUserData(0);
					_world->DestroyBody(body);

					actor->detach();					
				}
			}
			

			body = next;
		}
	}

	void circleClicked(Event *event)
	{
		TouchEvent *te = safeCast<TouchEvent*>(event);
		spActor actor = safeSpCast<Actor>(event->currentTarget);
		b2Body *body = (b2Body *)actor->getUserData();
		
		Vector2 dir = actor->getPosition() - actor->local2global(te->localPosition);
		dir.normalize();
		dir *= body->GetMass() * 200;

		body->ApplyForceToCenter(b2Vec2(dir.x, dir.y));


		//show click pos
		spSprite sprite = new Sprite();		
		sprite->setAnimFrame(gameResources.getResAnim("circle"));
		addChild(sprite);
		sprite->setScale(0.1f);
		sprite->setPosition(te->localPosition);
		sprite->setAnchor(Vector2(0.5f, 0.5f));
		actor->addChild(sprite);
	}

	void clicked(Event *event)
	{
		TouchEvent *te = safeCast<TouchEvent*>(event);

		spSprite sprite = new Sprite();		
		sprite->setAnimFrame(gameResources.getResAnim("circle"));
		addChild(sprite);
		sprite->setAnchor(Vector2(0.5f, 0.5f));
		sprite->setPosition(te->localPosition);

		sprite->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &MainActor::circleClicked));
		
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = convert(te->localPosition);

		b2Body *body = _world->CreateBody(&bodyDef);

		sprite->setUserData(body);

		float scale = (rand()%5)/10.0f + 0.5f;
		sprite->setScale(scale);

		b2CircleShape shape;
		shape.m_radius = sprite->getWidth() / SCALE / 2 * scale;

		

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;

		body->CreateFixture(&fixtureDef);
		body->SetUserData(sprite.get());
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