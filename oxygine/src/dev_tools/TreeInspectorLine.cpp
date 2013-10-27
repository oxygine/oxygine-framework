#include "TreeInspectorLine.h"
#include "TreeInspectorPage.h"
#include "TreeInspectorPreview.h"
#include "TreeInspector.h"
#include "TextActor.h"
#include "res/ResFont.h"
#include "res/Resources.h"

namespace oxygine
{
	
	TreeInspectorLine::TreeInspectorLine():_page(0)
	{
		setInputEnabled(false);
	}

	TreeInspectorLine::~TreeInspectorLine()
	{
	}


	void TreeInspectorLine::init(TreeInspectorPage *page, spActor item)
	{
		setSize(Vector2(0,0));
		setCull(true);

		_page = page;
		_item = new TreeInspectorPreview(page->getTreeInspector());
		_item->init(item);
		addChild(_item);

		
		spTextActor tb = new TextActor();
		tb->setInputEnabled(false);
		tb->setColor(Color(0,0,0,255));
	
		const float minWidth = 300.0f;

		TextStyle style;
		style.font = _page->getTreeInspector()->_resSystem->getResFont("system")->getFont();
		style.multiline = true;
		style.vAlign = TextStyle::VALIGN_TOP;
		tb->setStyle(style);
		tb->setSize(Vector2(minWidth, 0.0f));
		

		
		string desc = item->dump(0);

		tb->setHtmlText(desc);
		Vector2 ts = tb->getTextRect().size.cast<Vector2>();
		if (ts.x < minWidth)
			ts.x = minWidth;

		tb->setSize(ts);
		

		tb->setX(50.0f + 5);
	
		addChild(tb);

		setSize(0, 0);
		setSize(TreeInspector::calcBounds(this).size);
	}
}