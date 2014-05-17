#pragma once
#include "oxygine_include.h"
#include "oxygine.h"
#include <string>
#include <vector>

namespace oxygine
{
	using namespace std;

//#define OBJECT_POOL_ALLOCATOR 0

	class PoolObject
	{
	public:
#if OBJECT_POOL_ALLOCATOR
		void* operator new(size_t size);
		void operator delete(void *data, size_t size);
#endif
	};

	/**Base class for each oxygine object. Each object has unique internal ID and name. Debug build is tracking all created and deleted objects. Using memory pools*/	
	class ObjectBase: public PoolObject
	{
	public:
		ObjectBase(const ObjectBase &src);
		ObjectBase(bool assignID = true);
		virtual ~ObjectBase();

		const std::string&	getName() const;		
		void*				getUserData() const {return __userData;}
		int					getObjectID()const {return __id;}
		bool				isName(const string &name) const;
		bool				isName(const char *name) const;
		
		
		
		void setName(const string &name);		
		void setUserData(void *data){__userData = data;}
		
		void dumpObject() const;

		/**Dumps into Output all created and undeleted objects. Use it for memory leaks searching*/
		static void dumpCreatedObjects();		
		/**Shows assert when object with this unique ID will be created.*/
		static void showAssertInCtor(int id);
		/**Shows assert when object with this unique ID will be destroyed.*/
		static void showAssertInDtor(int id);
		
		//debug functions
		typedef vector<ObjectBase*> __createdObjects;
		static __createdObjects&	__getCreatedObjects();

		static void __startTracingLeaks();
		static void __stopTracingLeaks();
		void __removeFromDebugList();
		

	protected:
		static void __addToDebugList(ObjectBase *base);
		static void __removeFromDebugList(ObjectBase *base);
		void __generateID();

		mutable string *__name;//stl strings from marmalade allocate 16 bytes!!
		string *__getName() const;

		int __id;
		void *__userData;


#ifdef OXYGINE_DEBUG_TRACE_LEAKS
		bool __traceLeak;
#endif

		static int _lastID;
		static int _assertCtorID;
		static int _assertDtorID;
	};
	
	DECLARE_SMART(Object, spObject);
	class Object: public ref_counter, public ObjectBase
	{
	public:
		Object(const Object &src);
		Object(bool assignUniqueID = true);

	protected:
#ifdef OX_DEBUG
		int __check;
		void __doCheck();
#else
		void __doCheck(){}
#endif
	};
	
}