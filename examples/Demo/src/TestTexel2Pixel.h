#pragma once
#include "test.h"
class TestTexel2Pixel: public Test
{
public:
	TestTexel2Pixel()
	{
		spSprite sprite = new Sprite;			
		Vector2 pos(0, 0);
		sprite->setPosition(pos);
		sprite->setAnimFrame(resources.getResAnim("t2p"));
		sprite->attachTo(content);
		float scale = 1.0f/RootActor::instance->getScaleX();
		sprite->setScale(scale);
		Vector2 displaySpriteSize = sprite->getSize() * scale;
		sprite->setPosition((content->getSize() - displaySpriteSize)/2.0f);
	}
};
