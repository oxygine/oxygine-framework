#include "Restorable.h"
#include "Mutex.h"
#include <algorithm>

namespace oxygine
{
	Restorable::restorable _restorable;
	bool _restoring = false;
	Mutex _mutex;

	Restorable::restorable::iterator  findRestorable(Restorable *r)
	{
		Restorable::restorable::iterator i = std::find(_restorable.begin(), _restorable.end(), r);
		return i;
	}

	const Restorable::restorable &Restorable::getObjects()
	{
		return _restorable;
	}

	void Restorable::restoreAll()
	{
		restorable rs;
		swap(rs, _restorable);
		for (restorable::iterator i = rs.begin(); i != rs.end(); ++i)
		{
			Restorable *r = *i;
			r->restore();
		}
		//_restoring = false;
	}

	void Restorable::releaseAll()
	{
		restorable rs;
		swap(rs, _restorable);
		for (restorable::iterator i = rs.begin(); i != rs.end(); ++i)
		{
			Restorable *r = *i;
			r->release();
		}

		swap(rs, _restorable);
	}

	Restorable::Restorable(): _registered(false)
	{

	}

	Restorable::~Restorable()
	{
		unreg();
	}

	void Restorable::reg(RestoreCallback cb, void *user)
	{
		if (_registered)
			return;

		MutexAutoLock al(_mutex);

		OX_ASSERT(_restoring == false);
		_cb = cb;
		_userData = user;

		_registered = true;
		
		restorable::iterator i = findRestorable(this);
		OX_ASSERT(i == _restorable.end());		
		_restorable.push_back(this);
	}

	void Restorable::unreg()
	{
		if (!_registered)
			return;

		MutexAutoLock al(_mutex);
		OX_ASSERT(_restoring == false);
		restorable::iterator i = findRestorable(this);
		//OX_ASSERT(i != _restorable.end());
		if (i != _restorable.end())
		{
			_restorable.erase(i);
		}
		_registered = false;
	}

	void Restorable::restore()
	{
		if (!_cb)
			return;

		_cb(this, _userData);
	}
}