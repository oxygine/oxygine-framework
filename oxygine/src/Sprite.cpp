#include <sstream>
#include "Sprite.h"
#include "res/ResAnim.h"
#include "core/Renderer.h"
#include "RenderState.h"
#include "utils/stringUtils.h"
#include "RootActor.h"

namespace oxygine
{
	Sprite::Sprite()
	{

	}

	Sprite::~Sprite()
	{
	
	}

	Sprite::Sprite(const Sprite &src, cloneOptions opt):VStyleActor(src, opt)
	{
		_frame = src._frame;
		_vstyle= src._vstyle;
		if (getManageResAnim())
		{
			ResAnim *rs = _frame.getResAnim();
			if (rs)
				rs->getAtlas()->load();
		}
	}

	void Sprite::setManageResAnim(bool manage)
	{
		_flags &= ~flag_manageResAnim; 
		if (manage) 
			_flags |= flag_manageResAnim;
	}

	std::string Sprite::dump(const dumpOptions &options) const
	{
		stringstream stream;
		stream << "{Sprite}\n";
		stream << _vstyle.dump() << " ";
		string tname = "null";
		if (_frame.getDiffuse().base)
			tname = _frame.getDiffuse().base->getName();
		stream << "texture='" << tname << "' "; 
		if (_flags & flag_manageResAnim)
			stream << "manageResAnim=true"; 

		stream << Actor::dump(options);
		return stream.str();
	}

	void Sprite::setAnimFrame(const AnimationFrame &f)
	{
		changeAnimFrame(f);
	}

	void Sprite::setResAnim(const ResAnim *resanim)
	{
		if (resanim)
		{
			if (resanim->getTotalFrames()) 
				setAnimFrame(resanim);
			else
			{
				AnimationFrame fr;

				fr.init(0, Diffuse(), RectF(0,0,0,0), RectF(0,0,0,0), getSize());
				setAnimFrame(fr);
			}
		}
		else
			setAnimFrame(AnimationFrame());
	}

	void Sprite::setAnimFrame(const ResAnim *resanim, int col, int row)
	{
		//OX_ASSERT(resanim);
		if (!resanim)
		{
			changeAnimFrame(AnimationFrame());
			return;
		}

		const AnimationFrame &frame = resanim->getFrame(col, row);
		changeAnimFrame(frame);
	}

	void Sprite::changeAnimFrame(const AnimationFrame &frame)
	{
		if (_flags & flag_manageResAnim)
		{
			ResAnim *rs = _frame.getResAnim();
			if (rs)
				rs->getAtlas()->unload();

			rs = frame.getResAnim();
			if (rs)
				rs->getAtlas()->load();
		}		

		_frame = frame;
		setSize(_frame.getFrameSize());				

		animFrameChanged(_frame);
	}

	void Sprite::animFrameChanged(const AnimationFrame &f)
	{

	}

	RectF Sprite::getDestRect() const
	{
		if (_frame.getDiffuse().base)
			return calcDestRectF(_frame.getDestRect(), _frame.getFrameSize());

		return Actor::getDestRect();
	}
	

	void Sprite::doRender(const RenderState &rs)
	{		
		_vstyle._apply(rs);
		const Diffuse &df = _frame.getDiffuse();
		if (df.base)
		{
			rs.renderer->setDiffuse(df);

			RectF destRect = getDestRect();
			rs.renderer->draw(_frame.getSrcRect(), destRect);
		}		
	}


	TweenAnim::TweenAnim(const ResAnim *resAnim, int row):_resAnim(resAnim), _row(row), _colStart(0), _colEnd(0)
	{
		if (_resAnim)
			_colEnd = _resAnim->getColumns();
	}

	void TweenAnim::init(Sprite &actor)
	{

	}


	void TweenAnim::setResAnim(const ResAnim *resAnim)
	{
		_resAnim = resAnim;
		if (_resAnim)
			_colEnd = _resAnim->getColumns();
	}


	void TweenAnim::setColumns(int start, int end)
	{
		_colStart = start; _colEnd = end;
	}


	void TweenAnim::update(Sprite &actor, float p, const UpdateState &us)
	{
		OX_ASSERT(_resAnim);
		int col = interpolate<int>(_colStart, _colEnd, p);			

		if (_colStart > _colEnd)
		{
			if (col <= _colEnd)
				col = _colEnd + 1;
			OX_ASSERT(col <= _colStart && col > _colEnd);
		}
		else
		{
			if (col >= _colEnd)
				col = _colEnd - 1;
			OX_ASSERT(col >= _colStart && col < _colEnd);
		}		

		actor.setAnimFrame(_resAnim->getFrame(col, _row));
	}
}