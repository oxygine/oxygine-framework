#pragma once

#include "oxygine-framework.h"

#define JEWEL_SIZE 50

using namespace oxygine;

DECLARE_SMART(Jewel, spJewel);

typedef enum {jsNormal, jsAnimation, jsSelected, jsScored, jsDropped} TJewelState;

const string jewels_ids[6] = {"jewel1","jewel2","jewel3","jewel4","jewel5","jewel6"};

class Jewel : public Sprite
{
	Vector2 old_position;
	TJewelState state;
	int _id;
	bool locked;

public:
	Jewel(int jewel_id);

	Point index;
	void LockUnlock() {locked=!locked;}
	TJewelState getState() {return state;}
	void setState(TJewelState new_state) {state=new_state;}
	
	int GetId() {return _id;}
	
	void SetSelected();
	void UnSelect();
	spTween DropTo(Vector2 pos);
	bool IsID(spJewel spJewel) {return spJewel->GetId()==_id;}
	void Set(int id);

	spTween PlayAnimation();
	void AnimationEnd();	
};
