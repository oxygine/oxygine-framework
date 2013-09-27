#include "gameframe.h"	
#include "shared.h"


void Field::MakeTurn(bool changePlayer)
{


}

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
	//if (At(ind)->getState()!=jsNormal) return 0;
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
	//if (!At(ind)->IsID(jsNormal)) return 0;


	//определяем количество рядом стоящих одинаковых камней
	int s1=goLeft(ind);
	s1+=goRight(ind)-1;

	int s2=goUp(ind);
	s2+=goDown(ind)-1;

	int bs=0;


	if (s2>2 && s1>2) {//собирание камней по вертикали и горизонтали
		bs=s1+s2-1;
		if (donotscore) return bs;
		//Attack(stones[i][j].type,bs);

		At(ind)->LockUnlock();
		goLeft(ind,true); 
		goRight(ind,true); 
		goUp(ind,true);
		At(ind)->LockUnlock();
		goDown(ind,true);
		return bs;
	}

	if (s1>s2 && s1>2) {//собирание камней по горизонтали
		bs=s1-2;
		if (donotscore) return s1;
		//	Attack(stones[i][j].type,s1);

		At(ind)->LockUnlock();
		goLeft(ind,true); 
		At(ind)->LockUnlock();
		goRight(ind,true);
		return bs;
	}

	if (s2>s1 && s2>2) {//собирание камней по вертикали
		bs=s2-2;
		if (donotscore) return s2;
		//	Attack(stones[i][j].type,s2);
		At(ind)->LockUnlock();
		goUp(ind,true); 
		At(ind)->LockUnlock();
		goDown(ind,true);
		return bs;
	}
	return bs;
}

bool compareSolutions(const TFieldSolutions* left, const TFieldSolutions* right)
{
	return left->jewel_count > right->jewel_count;
};


bool Field::FindSolutions()
{
	std::vector<TFieldSolutions*> solutions;

	for(int i=0; i<FIELD_SIZE-1; i++)
		for(int j=0; j<FIELD_SIZE; j++)
		{
			int points;
			ForceSwap(jewels[i][j],jewels[i+1][j]);
			points = CheckJewel(Point(i,j),true);
			if (points>0) 
			{	
				solutions.push_back(new TFieldSolutions(Point(i,j),Point(i+1,j),points));
			}
			points += CheckJewel(Point(i+1,j),true);
			if (points>0) 
			{	
				solutions.push_back(new TFieldSolutions(Point(i+1,j),Point(i,j),points));
			}
			ForceSwap(jewels[i][j],jewels[i+1][j]);
		}

		for(int i=0; i<FIELD_SIZE; i++)
			for(int j=0; j<FIELD_SIZE-1; j++)
			{
				int points;
				ForceSwap(jewels[i][j],jewels[i][j+1]);
				points = CheckJewel(Point(i,j),true);
				if (points>0) 
				{	
					solutions.push_back(new TFieldSolutions(Point(i,j),Point(i,j+1),points));
				}
				points += CheckJewel(Point(i,j+1),true);
				if (points>0) 
				{	
					solutions.push_back(new TFieldSolutions(Point(i,j+1),Point(i,j),points));
				}
				ForceSwap(jewels[i][j],jewels[i][j+1]);
			}

			if (solutions.size()==0)
			{
				return false;
			}
			else
			{
				std::sort(solutions.begin(),solutions.end(), &compareSolutions);
				solutions.clear();
				return true;
			}


}


int compareVariants(const TFieldVariants* left, const TFieldVariants* right)
{
	return left->points > right->points;
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

	std::vector<TFieldVariants *> variants;
	for (int i=0; i<FIELD_SIZE; i++)
		for (int j=0; j<FIELD_SIZE; j++)
		{
			int points = CheckJewel(Point(i,j),true);
			if (points>0)
			{
				variants.push_back(new TFieldVariants(Point(i,j),points));
			}
		}

		if (variants.size()==0) 
		{
			variants.clear();
			return false;

		}
		else
		{
			//сортировка массива по убыванию
			std::sort(variants.begin(),variants.end(), &compareVariants);

			//собираем в поле самые лучшие варианты
			for (size_t i=0; i<variants.size(); i++)
			{
				CheckJewel(variants.at(i)->index);
				delete variants.at(i);
			}
			variants.clear();
			return true;
		}	
}


bool Field::BackSwap()
{
	if (back_swap) return false;
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
			else RefreshField();
		}
	}
}


bool Field::Swap(spTJewel First, spTJewel Second, bool skip_animation)
{
	Point diff = getCellIndex( First->getPosition()) -  getCellIndex( Second->getPosition());
	if (abs(diff.x)>1 || (abs(diff.y)>1) || abs(diff.x)==abs(diff.y) || Second==First || !( Second->getState()==jsNormal && First->getState()==jsNormal)) return false;

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

void Field::ForceSwap(spTJewel First, spTJewel Second)
{
	Point first_index = First->index;
	Point second_index = Second->index;

	jewels[first_index.x][first_index.y] = Second;
	jewels[first_index.x][first_index.y]->index = first_index;

	jewels[second_index.x][second_index.y] = First;
	jewels[second_index.x][second_index.y]->index = second_index;
}

void  Field::DropJewel(spTJewel Target, spTJewel Jewel)
{
	if (Jewel->getState()!=jsNormal) return;
	if (Target->getState()!=jsScored) return;


	spTween tween = Jewel->DropTo(Target->getPosition());
	tween->setDoneCallback(CLOSURE(this, &Field::DropEndCallback));		
	Target->setPosition( getCellPosition(Jewel->index.x, Jewel->index.y));
	ForceSwap(Target,Jewel);
	droped_count++;
}

void Field::RefreshField()
{	for (int i=0; i<FIELD_SIZE; i++)
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

	if (droped_count>0) state = fsAnimation;
	return true;
}

void Field::EndAnimationCallback(Event *ev)
{
	safeSpCast<Jewel>(ev->currentTarget)->AnimationEnd();
	animated_count--;

	if (animated_count==0) DropField();
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

Field::Field(Vector2 pos, Vector2 jewelsIndent)
{
	animated_count=0;
	droped_count=0;
	back_swap = false;

	EventCallback cb = CLOSURE(this, &Field::JewelClick);
	addEventListener(TouchEvent::TOUCH_DOWN, cb);
	state = fsWaiting;

	jewels_indent = jewelsIndent;
	setPosition(pos);
	setSize(Vector2(jewelsIndent.x*JEWEL_SIZE*FIELD_SIZE,jewelsIndent.y*JEWEL_SIZE*FIELD_SIZE));

	FillField(true);
	RefreshField();
}

spTJewel Field::At(Point index)
{
	if (index.x>-1 && index.y>-1 && index.x<FIELD_SIZE && index.y<FIELD_SIZE)
		return jewels[index.x][index.y];
	else
		return NULL;
}


Point Field::getCellIndex(Vector2 position)
{
	return Point((position.x)/(JEWEL_SIZE+jewels_indent.x),(position.y)/(JEWEL_SIZE+jewels_indent.y));
}

Vector2	Field::getCellPosition(int i, int j)
{
	return Vector2(((i*(JEWEL_SIZE+jewels_indent.x))),(j*(JEWEL_SIZE+jewels_indent.y)));
}


//////////////////////////////////////////////////////////////////////////////////////////

GameFrame::GameFrame()
{		
	setAnimFrame(res.getResAnim("bg"));

	spTField field = new Field(Vector2(107,42),Vector2(3,2));
	addChild(field);
}