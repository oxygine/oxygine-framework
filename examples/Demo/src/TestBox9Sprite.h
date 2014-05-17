#pragma once
#include "test.h"
#include "Box9Sprite.h"
class TestBox9Sprite: public Test
{
public:
	TestBox9Sprite()
	{
		ResAnim *rs = resources.getResAnim("box9a");
		spSprite original = new Sprite;
		original->setPosition(10, 70);
		original->setResAnim(rs);
		addChild(original);



		spBox9Sprite sprite;

		sprite = new Box9Sprite;			
		sprite->setPosition(270, 10);
		sprite->setResAnim(rs);
		sprite->setSize(300, 250);
		sprite->setVerticalMode(Box9Sprite::TILING_FULL);
		sprite->setHorizontalMode(Box9Sprite::TILING_FULL);

		//you could also define guides from xml  
		//<image file="box9.png" guideX1="50" guideX2="210" guideY1="50" guideY2="125" />
		sprite->setGuides(40, 160, 40, 160);
		addChild(sprite);


		sprite = new Box9Sprite;			
		sprite->setPosition(600, 10);
		sprite->setResAnim(rs);
		sprite->setSize(300, 250);
		//sprite->setVerticalMode(Box9Sprite::TILING_FULL);
		//sprite->setHorizontalMode(Box9Sprite::TILING_FULL);

		//you could also define guides from xml  
		//<image file="box9.png" guideX1="50" guideX2="210" guideY1="50" guideY2="125" />
		sprite->setGuides(40, 160, 40, 160);
		addChild(sprite);


		sprite = new Box9Sprite;			
		sprite->setPosition(getWidth()/4, 300);
		sprite->setResAnim(rs);
		sprite->setSize(getWidth()/2, 150);
		//sprite->setVerticalMode(Box9Sprite::TILING_FULL);
		//sprite->setHorizontalMode(Box9Sprite::TILING_FULL);

		//you could also define guides from xml  
		//<image file="box9.png" guideX1="50" guideX2="210" guideY1="50" guideY2="125" />
		sprite->setGuides(40, 160, 40, 160);
		addChild(sprite);


		sprite = new Box9Sprite;			
		sprite->setPosition(getWidth()/4, 480);
		sprite->setResAnim(rs);
		sprite->setSize(getWidth()/2, 150);
		sprite->setVerticalMode(Box9Sprite::TILING_FULL);
		sprite->setHorizontalMode(Box9Sprite::TILING_FULL);

		//you could also define guides from xml  
		//<image file="box9.png" guideX1="50" guideX2="210" guideY1="50" guideY2="125" />
		sprite->setGuides(40, 160, 40, 160);
		addChild(sprite);
	}
};
