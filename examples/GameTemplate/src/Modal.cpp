#include "Modal.h"
#include "Blocking.h"


namespace oxygine
{
	Modal::Modal()
	{

	}

	Modal::~Modal()
	{

	}	

	void Modal::showing()
	{
		_action = "";

		preShowing();				
		doShowing();		
		postShowing();
	}

	

	void Modal::hiding()
	{
		preHiding();		
		doHiding();
		postHiding();
	}

	string Modal::waitAction()
	{
		while (_action.empty())
		{
			blocking::yield();
		}

		string act = _action;
		_action = "";

		return act;
	}

	void Modal::generateActionByEvent(Event *ev)
	{
		generateAction(ev->currentTarget->getName());
	}

	bool Modal::generateAction(const string &action)
	{
		_action = action;
		return true;
	}


	void Modal::loop()
	{
		showing();
		doLoop();
		hiding();
	}
}