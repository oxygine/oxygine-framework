#include "jewels.h"
#include "shared.h"
#include "math.h"

Jewel::Jewel(int jewel_id)
{
	state = jsNormal;
	Set(jewel_id);
	setInputEnabled(false);
	setAnchor(0.5f, 0.5f);
}

void Jewel::Set(int id)
{
	setResAnim(res.getResAnim(jewels_ids[id]));
	_id = id;
	setAlpha(255);
}


void Jewel::UnSelect()
{
	if (state != jsSelected)
		return;

	state = jsNormal;
	setRotation(0);
	setPosition(old_position);
	removeTweens();
}

void Jewel::SetSelected()
{
	if (state != jsNormal)
		return;

	state = jsSelected;
	spTweenQueue tween = new TweenQueue();
	old_position = getPosition();
	//setPosition(getX() + JEWEL_SIZE/2, getY() + JEWEL_SIZE/2);

	tween->add(Sprite::TweenRotation(MATH_PI * 0.1f), 100);
	tween->add(Sprite::TweenRotation(MATH_PI  * (-0.1f)), 400,-1,true);


	addTween(tween);
}


spTween Jewel::PlayAnimation()
{
	if (!(state==jsNormal && locked) )
		return 0;

	state = jsAnimation;

	string anim = jewels_ids[_id];
	return addTween(createTween(Sprite::TweenAlpha(0), 500, 1));
}

spTween Jewel::DropTo(Vector2 pos)
{
	int time = fabs(getPosition().y - pos.y) * 2;
	spTween tween = createTween(Sprite::TweenPosition(pos),time);
	addTween( tween );
	state = jsDropped;
	return tween;
}

void Jewel::AnimationEnd()
{
	state = jsScored;
	setVisible(false);
}