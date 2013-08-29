#include "AtlasTool.h"
#include "core/Texture.h"

namespace oxygine
{
	AtlasNode::AtlasNode(AtlasNode *parent, const Rect &rect):_parent(parent), _rc(rect), _id(0)
	{
		_child[0] = 0;
		_child[1] = 0;
	}

	AtlasNode::~AtlasNode()
	{
		delete _child[0];
		delete _child[1];
	}

	AtlasNode *AtlasNode::insert(int width, int height)
	{
		AtlasNode *new_node = 0;
		if (_child[0] || _child[1])
		{
			new_node = _child[0]->insert(width, height);
			if (new_node)
				return new_node;

			new_node = _child[1]->insert(width, height);
			if (new_node)
				return new_node;

			return 0;
		}

		if (_id)
			return 0;

		if (width > _rc.getWidth())
			return 0;

		if (height > _rc.getHeight())
			return 0;

		if (width == _rc.getWidth() && height == _rc.getHeight())
			return this;

		_child[0] = new AtlasNode(this, _rc);
		_child[1] = new AtlasNode(this, _rc);

		int dw = _rc.getWidth() - width;
		int dh = _rc.getHeight() - height;

		_child[0]->_rc = _rc;
		_child[1]->_rc = _rc;

		if (dw > dh)
		{		
			_child[0]->_rc.setWidth(width);

			_child[1]->_rc.setX(_rc.getX() + width);
			_child[1]->_rc.setWidth(_rc.getWidth() - width);
		}
		else
		{
			_child[0]->_rc.setHeight(height);

			_child[1]->_rc.setY(_rc.getY() + height);
			_child[1]->_rc.setHeight(_rc.getHeight() - height);

		}

		return _child[0]->insert(width, height);
	}


	Atlas::Atlas():_tree(0), _bounds(0,0,0,0)
	{
		
	}

	Atlas::~Atlas()
	{
		clean();
	}

	void Atlas::clean()
	{
		delete _tree;
		_tree = 0;
	}

	void Atlas::init(int w, int h)
	{
		delete _tree;
		_tree = new AtlasNode(0, Rect(0,0, w, h));
	}

	bool Atlas::add(Texture *dest, const ImageData &src, Rect &srcRect)
	{
		AnimationFrame af;
		Point offset(1, 1);
		if (src.w == dest->getWidth())
			offset.x = 0;
		if (src.h == dest->getHeight())
			offset.y = 0;

		int w = src.w + offset.x;
		int h = src.h + offset.y;

		AtlasNode *node = _tree->insert(w, h);
		if (node)
		{
			node->setID(1);
			srcRect = Rect(node->getRect());

			srcRect.size = srcRect.size - offset;			
			dest->updateRegion(srcRect.pos.x, srcRect.pos.y, src);

			_bounds.unite(srcRect);

			return true;
		}

		return false;
	}
}