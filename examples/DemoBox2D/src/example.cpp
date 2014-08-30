#include "oxygine-framework.h"
#include "Box2D/Box2D.h"
#include "Box2DDebugDraw.h"

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


DECLARE_SMART(Circle, spCircle);
class Circle : public Sprite
{
public:
	Circle(b2World *world, const Vector2 &pos, float scale = 1)
	{
		setResAnim(gameResources.getResAnim("circle"));
		setAnchor(Vector2(0.5f, 0.5f));
		setTouchChildrenEnabled(false);

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = convert(pos);

		b2Body *body = world->CreateBody(&bodyDef);

		setUserData(body);

		setScale(scale);

		b2CircleShape shape;
		shape.m_radius = getWidth() / SCALE / 2 * scale;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;

		body->CreateFixture(&fixtureDef);
		body->SetUserData(this);
	}
};

DECLARE_SMART(Static, spStatic);
class Static : public Box9Sprite
{
public:
	Static(b2World *world, const RectF &rc)
	{
		//setHorizontalMode(Box9Sprite::TILING_FULL);
		//setVerticalMode(Box9Sprite::TILING_FULL);
		setResAnim(gameResources.getResAnim("pen"));
		setSize(rc.getSize());
		setPosition(rc.getLeftTop());
		setAnchor(Vector2(0.5f, 0.5f));

		b2BodyDef groundBodyDef;
		groundBodyDef.position = convert(getPosition());

		b2Body* groundBody = world->CreateBody(&groundBodyDef);

		b2PolygonShape groundBox;
		b2Vec2 sz = convert(getSize()/2);
		groundBox.SetAsBox(sz.x, sz.y);
		groundBody->CreateFixture(&groundBox, 0.0f);
	}
};

class MainActor: public Actor
{
public:	
	b2World *_world;
	spBox2DDraw _debugDraw;

	MainActor():_world(0)
	{	
		setSize(getStage()->getSize());

		spButton btn = new Button;
		btn->setResAnim(gameResources.getResAnim("button"));
		btn->setX(getWidth() - btn->getWidth() - 3);
		btn->setY(3);
		btn->attachTo(this);
		btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::showHideDebug));

		addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::click));


		_world = new b2World(b2Vec2(0, 10), false);


		spStatic ground = new Static(_world, RectF(getWidth() / 2, getHeight() - 10, getWidth() - 100, 30));
		addChild(ground);

		spCircle circle = new Circle(_world, getSize()/2, 2);
		addChild(circle);
	}
	
	void doUpdate(const UpdateState &us)
	{
		//in real project you should make steps with fixed dt, check box2d documentation
		_world->Step(us.dt / 1000.0f, 6, 2);

		//update each body position on display
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

				//remove fallen bodies
				if (actor->getY() > getHeight() + 50)
				{
					body->SetUserData(0);
					_world->DestroyBody(body);

					actor->detach();					
				}
			}			

			body = next;
		}
	}

	void showHideDebug(Event *event)
	{
		TouchEvent *te = safeCast<TouchEvent*>(event);
		te->stopsImmediatePropagation = true;
		if (_debugDraw)
		{
			_debugDraw->detach();
			_debugDraw = 0;
			return;
		}

		_debugDraw = new Box2DDraw;		
		_debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
		_debugDraw->attachTo(this);
		_debugDraw->setWorld(SCALE, _world);
		_debugDraw->setPriority(1);
	}

	void click(Event *event)
	{
		TouchEvent *te = safeCast<TouchEvent*>(event);
				
		if (event->target.get() == this)
		{
			spCircle circle = new Circle(_world, te->localPosition);
			circle->attachTo(this);
		}

		if (event->target->getUserData())
		{
			//shot to circle
			spActor actor = safeSpCast<Actor>(event->target);
			b2Body *body = (b2Body *)actor->getUserData();

			Vector2 dir = actor->getPosition() - te->localPosition;
			dir = dir / dir.length() * body->GetMass() * 200;

			body->ApplyForceToCenter(b2Vec2(dir.x, dir.y));

			spSprite sprite = new Sprite();
			sprite->setResAnim(gameResources.getResAnim("shot"));
			Vector2 local = actor->global2local(te->localPosition);
			sprite->setPosition(local);
			sprite->setAnchor(Vector2(0.5f, 0.5f));
			sprite->attachTo(actor);
		}
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
	//and add it to Stage as child
	getStage()->addChild(actor);
}

void example_destroy()
{
	gameResources.free();
}

void example_update()
{

}