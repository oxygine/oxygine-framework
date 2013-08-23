#pragma once
#include "oxygine-framework.h"

namespace oxygine
{
	DECLARE_SMART(Modal, spModal);
	class Modal: public Object
	{
	public:
		Modal();
		~Modal();
		
		void generateActionByEvent(Event *ev);
		bool generateAction(const string &action);

		void loop();
		string waitAction();

		virtual void showing();
		virtual void preShowing(){}
		virtual void doShowing(){}
		virtual void postShowing(){}

		virtual void doLoop(){}

		virtual void hiding();
		virtual void preHiding(){}
		virtual void doHiding(){}
		virtual void postHiding(){}		

	protected:		
		string _action;
	};
}
