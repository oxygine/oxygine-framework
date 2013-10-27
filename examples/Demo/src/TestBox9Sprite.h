#pragma once
#include "test.h"
#include "Box9Sprite.h"
class TestBox9Sprite: public Test
{
public:
	TestBox9Sprite()
	{
		ResAnim *rs = resources.getResAnim("box9");
		spSprite original = new Sprite;
		original->setPosition(10, 70);
		original->setResAnim(rs);
		addChild(original);

		spBox9Sprite sprite = new Box9Sprite;			
		sprite->setPosition(270, 70);
		sprite->setResAnim(rs);
		sprite->setSize(300, 400);
		//sprite->setVerticalMode(Box9Sprite::TILING_FULL);
		//sprite->setHorizontalMode(Box9Sprite::TILING);

		//you could also define guides from xml  
		//<image file="box9.png" guideX1="50" guideX2="210" guideY1="50" guideY2="125" />
		sprite->setGuides(50, 210, 50, 125);
		addChild(sprite);
	}
};
