#pragma once
#include "oxygine_include.h"
#include "Sprite.h"
namespace oxygine
{
	DECLARE_SMART(Button, spButton);

	class ResAnim;

	class Button: public Sprite
	{
	public:
		DECLARE_COPYCLONE_NEW(Button);

		Button();
		~Button();

		int	getRow() const {return _row;}
		//spEventHandler getHandler() {return _ph;}

		void setResAnim(const ResAnim *r);
		/**setAnimFrame is identical to setResAnim*/
		//void setAnimFrame(const ResAnim *resanim, int col, int row);

		//void setCallbackClick(EventCallback cb);		

		/**Sets which row from ResAnim should be used. Default value is 0. Could be used for CheckBoxes*/
		void setRow(int row);

	protected:
		enum state
		{
			stateNormal,			
			stateOvered,
			statePressed 
		};		
		virtual void updateButtonState(state s);

		const ResAnim *_resAnim;
		int _row;
		state _state;

	private:
		pointer_index _btnPressed;
		pointer_index _btnOvered;

		void _mouseEvent(Event *event);
		void _mouseUpOutside(Event *event);

		void setState(state s);	
	};
}