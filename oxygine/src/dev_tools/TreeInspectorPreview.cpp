#include "TreeInspectorPreview.h"
#include "TreeInspector.h"
#include "RootActor.h"
#include "TextField.h"
#include "res/Resources.h"
#include "RenderState.h"

namespace oxygine
{
	TreeInspectorPreview::TreeInspectorPreview(TreeInspector *tree): _tree(tree), _prevParent(0), _drawChildren(false)
	{
		setInputEnabled(false);
	}

	TreeInspectorPreview::~TreeInspectorPreview()
	{

	}

	Vector2 fitSize(const Vector2 &destSize, const Vector2 &src)
	{
		Vector2 srcSize = src;
		if (srcSize.x < 1)
			srcSize.x = 1;
		if (srcSize.y < 1)
			srcSize.y = 1;

		float scaleX = srcSize.x / destSize.x;
		float scaleY = srcSize.y / destSize.y;

		scaleX = 1.0f / scaleX;
		scaleY = 1.0f / scaleY;

		float scale = scaleX < scaleY ? scaleX : scaleY;		
		return Vector2(srcSize.x * scale, srcSize.y * scale);
	}

	void TreeInspectorPreview::init(spActor item)
	{
		//_item = item;
		Renderer r(&_videoCache);
		RenderState rs;
		rs.renderer = &r;
		rs.transform = item->getTransform();
		r.begin(0, Rect(0,0,0,0), 0);
		r.setTransform(rs.transform);
		item->doRender(rs);
		r.end();
		r.drawBatch();

		setSize(30, 30);

		RectF itemRect = _videoCache._bounds;
		if (itemRect.isEmpty())
		{
			itemRect = item->getDestRect();
			if (itemRect.isEmpty())
				itemRect.setSize(Vector2(10,4));
		}

		Vector2 ns = fitSize(Vector2(50.0f, 50.0f), itemRect.size); 
		float scale = ns.x / itemRect.size.x;

		_cacheTransform.identity();
		
		_cacheTransform.scale(Vector2(scale, scale));
		_cacheTransform.translate(-itemRect.pos);
		


		AnimationFrame fr = _tree->_resSystem->getResAnim("checker")->getFrame(0,0);
		//Point itemSize(30, 30);// = _getItemRect().size;

		RectF srcRect = fr.getSrcRect();
		const Diffuse &df = fr.getDiffuse();
		srcRect.size.x = ns.x / (float)df.base->getWidth();
		srcRect.size.y = ns.y / (float)df.base->getHeight();
		RectF destRect = fr.getDestRect();
		destRect.size = ns;

		AnimationFrame cfr;
		cfr.init(0, df, srcRect, destRect, ns);
		
		setAnimFrame(cfr);

		
		/*
		spEventHandler bh = new EventHandler();

		bh->setCallbackEnter(CLOSURE(this, &TreeInspectorPreview::_onEvent));
		bh->setCallbackExit(CLOSURE(this, &TreeInspectorPreview::_onEvent));	
		bh->setCallbackPressDown(CLOSURE(this, &TreeInspectorPreview::_onEvent));
		bh->setCallbackPressUp(CLOSURE(this, &TreeInspectorPreview::_onEvent));
		addEventHandler(bh);
		*/
	}

	void TreeInspectorPreview::doRender(RenderState const& parentRenderState)
	{
		Sprite::doRender(parentRenderState);

		parentRenderState.renderer->drawBatch();
		_videoCache.render(_cacheTransform * parentRenderState.transform);
		parentRenderState.renderer->drawBatch();
		parentRenderState.renderer->resetSettings();

		/*
		bool rel = _item->getChildrenRelative();

		AffineTransform tr;
		tr.identity();
		Rect rect = _getItemRect();
		float scale = (float)getWidth() / rect.size.x;
		tr.scale(Vector2(scale, scale));
		tr.translate(-rect.pos);
		
		AffineTransform res = tr * parentRenderState.transform;
		_item->setChildrenRelative(true);
		parentRenderState.renderer->setTransform(res);
		_item->doRender(parentRenderState);
		_item->setChildrenRelative(rel);		
		*/
	}
}