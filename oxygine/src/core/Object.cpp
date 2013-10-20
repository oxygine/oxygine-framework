#include "Object.h"
#include "oxygine.h"
#include <typeinfo>
#include <stdio.h>
#include "log.h"
#include "Mutex.h"
#include "utils/stringUtils.h"
#include <string.h>

namespace oxygine
{
	ObjectBase::createdObjects ObjectBase::__objects;

	int ObjectBase::_lastID = 0;
	int ObjectBase::_assertCtorID = -1;	
	int ObjectBase::_assertDtorID = -1;	

	
	Mutex mutexDebugList;

#if OBJECT_POOL_ALLOCATOR
	void* PoolObject::operator new(size_t size)
	{
		return fastAlloc(size);			 
	}

	void PoolObject::operator delete(void *data, size_t size)
	{
#ifdef OX_DEBUG
		memset(data, 0xCDCDCDCD, size);
#endif
        
		fastFree(data);
	}
#endif

	string *ObjectBase::__getName() const
	{
		if (!__name)
		{
			__name = (string*)fastAlloc(sizeof(string));
			new (__name) string;			
		}
		return __name;
	}

	const std::string &ObjectBase::getName() const
	{
		return *__getName();
	}

	bool ObjectBase::isName(const string &name) const
	{
		if (__name && *__name == name)
			return true;
		return false;
	}

	bool ObjectBase::isName(const char *name) const
	{
		if (__name && !strcmp(__name->c_str(), name))
			return true;
		return false;
	}

	void ObjectBase::setName(const string &name)
	{
		string *n = __getName();
		*n = name;
	}

	ObjectBase::ObjectBase(const ObjectBase &src):__id(0), __name(0)
	{
#ifdef OXYGINE_DEBUG_OBJECTS
		{
			MutexAutoLock m(mutexDebugList);
			__objects.push_back(this);
		}		
#endif

		__userData = src.__userData;
		if (src.__name)
			setName(*src.__name);
		__generateID();
	}

	void ObjectBase::__generateID()
	{
		__id = ++_lastID;

		//OX_ASSERT(_id != 889);
		OX_ASSERT(_assertCtorID != __id);
	}

	ObjectBase::ObjectBase(bool assignID):__userData(0), __name(0), __id(0)
	{
#ifdef OXYGINE_DEBUG_OBJECTS
		{
			MutexAutoLock m(mutexDebugList);
			__objects.push_back(this);
		}		
#endif

		if (assignID)
			__generateID();
	}

	void ObjectBase::showAssertInCtor(int id)
	{
		_assertCtorID = id;
	}

	void ObjectBase::showAssertInDtor(int id)
	{
		_assertDtorID = id;
	}

	ObjectBase::~ObjectBase()
	{
		OX_ASSERT(_assertDtorID != __id);
		__removeFromDebugList(this);

		if (__name)
		{
			__name->~string();
			fastFree(__name);		
		}
	}

	void ObjectBase::__removeFromDebugList(ObjectBase *base)
	{
#ifdef OXYGINE_DEBUG_OBJECTS
		{
			MutexAutoLock m(mutexDebugList);
			createdObjects::iterator i = std::find(__objects.begin(), __objects.end(), base);
			OX_ASSERT(i != __objects.end());
			__objects.erase(i);	
		}		
#endif
	}

	void ObjectBase::dumpObject() const
	{
		const Object *o = dynamic_cast<const Object*>(this);
		string name;

		char refs[16] = "-";
		if (o)
		{
			safe_sprintf(refs, "%d", o->_ref_counter);
			if (o->__name)
				name = *o->__name;
		}

				
		log::messageln("id = %d, name = '%s', typeid = '%s', refs = %s", this->__id, name.c_str(), typeid(*this).name(), refs);
	}

	void ObjectBase::dumpCreatedObjects()
	{
#ifdef OXYGINE_DEBUG_OBJECTS
		MutexAutoLock m(mutexDebugList);

		log::messageln("\n\n\nallocated objects:");
		int n = 0;		
		for (createdObjects::iterator i = __objects.begin(); i != __objects.end(); ++i)
		{
			ObjectBase *object = *i;
			//log::message("%d)", n);
			object->dumpObject();			

			++n;
		}
		log::message("total: %d -----------------------------\n\n\n", (int)__objects.size());
#endif
	}

	Object::Object(const Object &src):ObjectBase(src)
	{
#ifdef OX_DEBUG
		__check = 0xABCDEFAB;
#endif
	}

	Object::Object(bool assignUniqueID):ObjectBase(assignUniqueID)
	{
#ifdef OX_DEBUG
		__check = 0xABCDEFAB;
#endif
	}
#ifdef OX_DEBUG
	void Object::__doCheck()
	{
		OX_ASSERT(__check == 0xABCDEFAB);
	}
	#endif
}