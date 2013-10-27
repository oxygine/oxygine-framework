#include "gameframe.h"	
#include "shared.h"


int Field::StartAnimation(Point ind)
{
	spTween tween = At(ind)->PlayAnimation();
	if (tween)
	{
		tween->setDoneCallback(CLOSURE(this, &Field::EndAnimationCallback));
		animated_count++;
	}
	return 0;
}

int Field::goUp(Point ind, bool scoreit)
{	

	Point next_ind = ind;
	next_ind.y-=1;
	int sum=1;

	if (At(next_ind) && At(next_ind)->IsID(At(ind))) sum+=goUp(next_ind,scoreit);
	if (scoreit) 
	{
		StartAnimation(ind);
	}
	return sum; 
}

int Field::goDown(Point ind, bool scoreit)
{

	Point next_ind = ind;
	next_ind.y+=1;
	int sum=1;

	if (At(next_ind) && At(next_ind)->IsID(At(ind))) sum+=goDown(next_ind,scoreit);
	if (scoreit) 
	{
		StartAnimation(ind);
	}
	return sum; 
}

int Field::goRight(Point ind, bool scoreit)
{

	Point next_ind = ind;
	next_ind.x+=1;
	int sum=1;

	if (At(next_ind) && At(next_ind)->IsID(At(ind))) sum+=goRight(next_ind,scoreit);
	if (scoreit) 
	{
		StartAnimation(ind);
	}		
	return sum;
}


int Field::goLeft(Point ind, bool scoreit)
{
	Point next_ind = ind;
	next_ind.x-=1;
	int sum=1;

	if (At(next_ind) && At(next_ind)->IsID(At(ind)))  sum+=goLeft(next_ind,scoreit);
	if (scoreit) 
	{
		StartAnimation(ind);
	}
	return sum;
}

int Field::CheckJewel(Point ind, bool donotscore)
{
	int s1=goLeft(ind);
	s1+=goRight(ind)-1;

	int s2=goUp(ind);
	s2+=goDown(ind)-1;

	int bs=0;


	if (s2>2 && s1>2)
	{
		bs=s1+s2-1;
		if (donotscore)
			return bs;

		At(ind)->LockUnlock();
		goLeft(ind,true); 
		goRight(ind,true); 
		goUp(ind,true);
		At(ind)->LockUnlock();
		goDown(ind,true);
		return bs;
	}

	if (s1>s2 && s1>2)
	{
		bs=s1-2;
		if (donotscore) 
			return s1;

		At(ind)->LockUnlock();
		goLeft(ind,true); 
		At(ind)->LockUnlock();
		goRight(ind,true);
		return bs;
	}

	if (s2>s1 && s2>2)
	{
		bs=s2-2;
		if (donotscore) 
			return s2;
		At(ind)->LockUnlock();
		goUp(ind,true); 
		At(ind)->LockUnlock();
		goDown(ind,true);
		return bs;
	}
	return bs;
}


bool Field::FindSolutions()
{
	std::vector<TFieldSolutions> solutions;

	for(int i=0; i<FIELD_SIZE-1; i++)
	{
		for(int j=0; j<FIELD_SIZE; j++)
		{
			int points;
			ForceSwap(jewels[i][j],jewels[i+1][j]);
			points = CheckJewel(Point(i,j),true);
			if (points>0) 
			{	
				solutions.push_back(TFieldSolutions(Point(i,j),Point(i+1,j),points));
			}
			points += CheckJewel(Point(i+1,j),true);
			if (points>0) 
			{	
				solutions.push_back(TFieldSolutions(Point(i+1,j),Point(i,j),points));
			}
			ForceSwap(jewels[i][j],jewels[i+1][j]);
		}
	}

	for(int i=0; i<FIELD_SIZE; i++)
	{
			for(int j=0; j<FIELD_SIZE-1; j++)
			{
				int points;
				ForceSwap(jewels[i][j],jewels[i][j+1]);
				points = CheckJewel(Point(i,j),true);
				if (points>0) 
				{	
					solutions.push_back(TFieldSolutions(Point(i,j),Point(i,j+1),points));
				}
				points += CheckJewel(Point(i,j+1),true);
				if (points>0) 
				{	
					solutions.push_back(TFieldSolutions(Point(i,j+1),Point(i,j),points));
				}
				ForceSwap(jewels[i][j],jewels[i][j+1]);
			}
	}

	if (solutions.empty())	
		return false;
	
	return true;
}


void Field::FillField(bool first_time)
{
	for (int i=0; i<FIELD_SIZE; i++)
		for (int j=FIELD_SIZE-1; j>=0; j--) 
		{
			if (first_time)
			{
				jewels[i][j] = new Jewel(rand()%6);
				jewels[i][j]->setPosition( getCellPosition(i,j) );
				jewels[i][j]->index = Point(i,j);
				addChild(jewels[i][j]);
			}
			else
				jewels[i][j]->Set(rand()%6);
		}

		CleanField();
}

void Field::CleanField()
{
	bool ex;
	do
	{
		ex = true;
		for(int i=0; i<FIELD_SIZE; i++)
			for(int j=0; j<FIELD_SIZE; j++)
			{
				if (CheckJewel(Point(i,j),true)>0)
				{
					jewels[i][j]->Set(rand()%6);
					ex=false;
				}	
			}
	}while(!ex);
}

bool Field::CheckField()
{
	std::vector<TFieldVariants> variants;
	for (int i=0; i<FIELD_SIZE; i++)
		for (int j=0; j<FIELD_SIZE; j++)
		{
			int points = CheckJewel(Point(i,j),true);
			if (points>0)
			{
				variants.push_back(TFieldVariants(Point(i,j),points));
			}
		}

		if (variants.empty()) 
		{
			return false;
		}
		else
		{
			for (size_t i=0; i<variants.size(); i++)
			{
				CheckJewel(variants.at(i).index);
			}
			variants.clear();
			return true;
		}	
}


bool Field::BackSwap()
{
	if (back_swap) 
		return false;
	back_swap = true;
	Swap( At(jewel_drag_ind), At(jewel_new_ind));
	return true;
}


void Field::DropEndCallback(Event *ev)
{
	safeSpCast<Jewel>(ev->currentTarget)->setState(jsNormal);
	droped_count--;
	if (droped_count==0) 
	{
		if (CheckField()) 
			DropField();
		else
		{
			if (FindSolutions())
				state = fsWaiting;
			else 
				RefreshField();
		}
	}
}


bool Field::Swap(spJewel First, spJewel Second, bool skip_animation)
{
	Point diff = getCellIndex(First->getPosition()) - getCellIndex(Second->getPosition());
	if (abs(diff.x)>1 || (abs(diff.y)>1) || 
		abs(diff.x)==abs(diff.y) || Second==First || 
		!(Second->getState()==jsNormal && First->getState()==jsNormal)) 
		return false;

	ForceSwap(First, Second);

	if (!skip_animation)
	{
		state = fsAnimation;

		jewel_new_ind  = getCellIndex( First->getPosition());
		jewel_drag_ind = getCellIndex( Second->getPosition());

		First->addTween( createTween(Sprite::TweenPosition(Second->getPosition()),200));
		spTween tween = createTween(Sprite::TweenPosition(First->getPosition()),200);
		tween->setDoneCallback(CLOSURE(this, &Field::EndSwapCallback));
		Second->addTween(tween);
	}

	return true;
}

void Field::JewelClick(Event *event)
{
	TouchEvent *te = safeCast<TouchEvent*>(event);
	Point ind = getCellIndex(te->localPosition);

	if (state==fsSwaping)
	{
		if (At(ind))
		{
			state = fsWaiting;
			At(jewel_drag_ind)->UnSelect();
			At(ind)->UnSelect();
			back_swap = false;
			Swap(At(ind),At(jewel_drag_ind));
		}
	}

	if (state==fsWaiting)
	{
		if (At(ind))
		{
			//At(ind)->PlayAnimation();
			jewel_drag_ind = ind;
			At(ind)->SetSelected();
			state = fsSwaping;
		}
	}

	DropField();
}

int Field::FindFreeCell(int column, int limit)
{
	int free = -1;
	for (int j=FIELD_SIZE-1; j>limit; j--) 
		if (jewels[column][j]->getState()==jsScored)
		{
			free = j;
			break;
		}

		return free;
}

void Field::ForceSwap(spJewel First, spJewel Second)
{
	Point first_index = First->index;
	Point second_index = Second->index;

	jewels[first_index.x][first_index.y] = Second;
	jewels[first_index.x][first_index.y]->index = first_index;

	jewels[second_index.x][second_index.y] = First;
	jewels[second_index.x][second_index.y]->index = second_index;
}

void  Field::DropJewel(spJewel Target, spJewel Jewel)
{
	if (Jewel->getState()!=jsNormal)
		return;

	if (Target->getState()!=jsScored) 
		return;

	spTween tween = Jewel->DropTo(Target->getPosition());
	tween->setDoneCallback(CLOSURE(this, &Field::DropEndCallback));		
	Target->setPosition(getCellPosition(Jewel->index.x, Jewel->index.y));
	ForceSwap(Target,Jewel);
	droped_count++;
}

void Field::RefreshField()
{	
	for (int i=0; i<FIELD_SIZE; i++)
	{
		for (int j=FIELD_SIZE-1; j>=0; j--) 
		{
			jewels[i][j]->setState(jsScored);
		}
		GenerateNewJewels(i);
	}
	CleanField();
}

int Field::GenerateNewJewels(int column)
{
	int count = 0;
	for (int j=FIELD_SIZE-1; j>=0; j--) 
	{
		if (jewels[column][j]->getState()==jsScored)
		{
			Vector2 pos = jewels[column][j]->getPosition();
			jewels[column][j]->Set(rand()%6);
			jewels[column][j]->setVisible(true);
			jewels[column][j]->setPosition(Vector2(pos.x,-getPosition().y-count*(JEWEL_SIZE + rand() % 10 + 20)));
			spTween tween = jewels[column][j]->DropTo(pos);
			droped_count++;
			tween->setDoneCallback(CLOSURE(this, &Field::DropEndCallback));		
			count++;

		}
	}

	return 0;
}

bool Field::DropField()
{
	for (int i=0; i<FIELD_SIZE; i++)
	{
		for (int j=FIELD_SIZE-1; j>=0; j--) 
		{
			if (jewels[i][j]->getState()==jsNormal)
			{
				int free = FindFreeCell(i,j);
				if (free!=-1) 
				{
					DropJewel(jewels[i][free],jewels[i][j]);
				}
			}
		}
		GenerateNewJewels(i);
	}

	if (droped_count>0)
		state = fsAnimation;
	return true;
}

void Field::EndAnimationCallback(Event *ev)
{
	safeSpCast<Jewel>(ev->currentTarget)->AnimationEnd();
	animated_count--;

	if (animated_count==0) 
		DropField();
}

void Field::EndSwapCallback(Event *ev)
{
	if (CheckField())
	{
		state = fsAnimation;		
	}
	else
	{
		if (!BackSwap()) state = fsWaiting;
	}
}

Field::Field()
{
	animated_count=0;
	droped_count=0;
	back_swap = false;

	EventCallback cb = CLOSURE(this, &Field::JewelClick);
	addEventListener(TouchEvent::TOUCH_DOWN, cb);
	state = fsWaiting;

	setSize(JEWEL_SIZE * FIELD_SIZE, JEWEL_SIZE * FIELD_SIZE);

	FillField(true);
	RefreshField();
}

spJewel Field::At(Point index)
{
	if (index.x>-1 && index.y>-1 && index.x<FIELD_SIZE && index.y<FIELD_SIZE)
		return jewels[index.x][index.y];
	else
		return NULL;
}


Point Field::getCellIndex(Vector2 position)
{
	return Point(position.x/JEWEL_SIZE, position.y/JEWEL_SIZE);
}

Vector2	Field::getCellPosition(int i, int j)
{
	return Vector2(i * JEWEL_SIZE + JEWEL_SIZE/2, j * JEWEL_SIZE + JEWEL_SIZE/2);
}

//////////////////////////////////////////////////////////////////////////////////////////

GameFrame::GameFrame()
{		
	setResAnim(res.getResAnim("bg"));

	spTField field = new Field();
	float scale = getRoot()->getHeight() * 0.9f / field->getHeight();
	field->setScale(scale);

	field->setPosition(getRoot()->getSize()/2 - field->getSize() * scale/2);
	addChild(field);
}