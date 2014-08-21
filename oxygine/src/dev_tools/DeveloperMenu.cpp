#include "DeveloperMenu.h"
#include "ColorRectSprite.h"
#include "ClipRectActor.h"
#include "TextField.h"
#include "DebugActor.h"
#include "res/Resources.h"
#include "initActor.h"

namespace oxygine
{
	const Color windowColor(64, 128, 128, 255);

	DeveloperMenu::DeveloperMenu():_resSystem(0)
	{
		setName(getDefaultName());

		setColor(windowColor);

		DebugActor::initialize();
		_resSystem = DebugActor::resSystem;

		setPriority(999);
	}

	DeveloperMenu::~DeveloperMenu()
	{

	}

	Vector2 DeveloperMenu::getBodySize() const
	{
		return _body->getSize();
	}

	void DeveloperMenu::init(const Vector2 &size, const char *text, spActor data, const Color &color)
	{
		const float BORDER = 3;

		float offset = 24;

		setSize(size);
		setWidth(data->getWidth() + BORDER * 2);
		setHeight(size.y + offset);
		//setSize(data->getSize() + Point(0, offset));

		spColorRectSprite top = new ColorRectSprite();
		top->setWidth(getWidth());
		top->setSize(size);


		_topDrag.init(top.get());
		_topDrag.setDragClient(this);
		
		addChild(top);

		

		top->setColor(windowColor);
		top->setSize(Vector2(getWidth(), offset));
		top->setPriority(100);

		spColorRectSprite body = new ColorRectSprite();
		body->setColor(color);
		addChild(body);

		

		body->setSize(Vector2(getWidth() - BORDER * 2, getHeight()));
		body->setX(BORDER);
		body->setY((float)offset);


		_body = body;	


		TextStyle style;
		style.font = _resSystem->getResFont("system")->getFont();
		style.vAlign = TextStyle::VALIGN_TOP;

		spButton close = initActor(new Button,
			arg_y = offset/2,
			arg_x = offset/2,
			arg_anchor = Vector2(0.5f, 0.5f),
			arg_resAnim = _resSystem->getResAnim("remove"),
			arg_attachTo = top);
		close->addEventListener(TouchEvent::CLICK, CLOSURE(this, &DeveloperMenu::close));


		spTextField title = initActor(new TextField,
			arg_style = style,
			arg_hAlign = TextStyle::HALIGN_MIDDLE,
			arg_vAlign = TextStyle::VALIGN_MIDDLE,
			arg_text = text,
			arg_width = top->getWidth(),
			arg_height = top->getHeight(),
			arg_input = false,
			arg_attachTo = top);

		//title->setSize(close->getTextRect().size);
		title->addEventListener(TouchEvent::CLICK, CLOSURE(this, &DeveloperMenu::close));

		data->attachTo(body);
	}


	void DeveloperMenu::close(Event *ev)
	{
		detach();
		//return true;
	}
}