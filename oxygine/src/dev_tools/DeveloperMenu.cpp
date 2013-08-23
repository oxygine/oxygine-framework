#include "DeveloperMenu.h"
#include "ColorRectSprite.h"
#include "ClipRectActor.h"
#include "TextActor.h"
#include "DebugActor.h"
#include "res/Resources.h"
#include "InitActor.h"

namespace oxygine
{
	DeveloperMenu::DeveloperMenu():_resSystem(0)
	{
		setName(getDefaultName());

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
		setSize(size);
		setWidth(data->getWidth());
		//setSize(data->getSize() + Point(0, offset));

		spColorRectSprite top = new ColorRectSprite();
		top->setWidth(getWidth());

		_topDrag.init(top.get());
		_topDrag.setDragClient(this);
		
		addChild(top);
		float offset = 24;

		top->setColor(Color(64, 128, 128, 255));
		top->setSize(Vector2(getWidth(), offset));
		top->setPriority(100);

		spColorRectSprite body = new ColorRectSprite();
		body->setColor(color);
		addChild(body);

		body->setSize(Vector2(getWidth(), getHeight()));
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


		spTextActor title = initActor(new TextActor,
			arg_style = style,
			arg_hAlign = TextStyle::HALIGN_CENTER,
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