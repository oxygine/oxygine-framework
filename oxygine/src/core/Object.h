#pragma once
#include "oxygine-include.h"
#include "ref_counter.h"
#include <string>
#include <vector>

namespace oxygine
{
//#define OBJECT_POOL_ALLOCATOR 0

    class PoolObject
    {
    public:
#if OBJECT_POOL_ALLOCATOR
        void* operator new(size_t size);
        void operator delete(void* data, size_t size);
#endif
    };

    /**Base class for each oxygine object. Each object has unique internal ID and name. Debug build is tracking all created and deleted objects. Using memory pools*/
    class ObjectBase: public PoolObject
    {
    public:
        ObjectBase(const ObjectBase& src);
        ObjectBase(bool assignID = true);
        virtual ~ObjectBase();

        const std::string&  getName() const;
        const void*         getUserData() const {return __userData;}
        int                 getObjectID()const {return __id;}
        bool                isName(const std::string& name) const;
        bool                isName(const char* name) const;



        void setName(const std::string& name);
        void setUserData(const void* data) {__userData = data;}

        void dumpObject() const;

        /**Dumps into Output all created and undeleted objects. Use it for memory leaks searching*/
        static void dumpCreatedObjects();
        /**Shows assert when object with this unique ID will be created.*/
        static void showAssertInCtor(int id);
        /**Shows assert when object with this unique ID will be destroyed.*/
        static void showAssertInDtor(int id);

        //debug functions
        typedef std::vector<ObjectBase*> __createdObjects;
        static __createdObjects&    __getCreatedObjects();

        static void __startTracingLeaks();
        static void __stopTracingLeaks();
        void __removeFromDebugList();


    protected:
        static void __addToDebugList(ObjectBase* base);
        static void __removeFromDebugList(ObjectBase* base);
        void __generateID();

#if     DYNAMIC_OBJECT_NAME
        //some objects dont need name
        mutable std::string* __name;
#else
        mutable std::string __name;
#endif
        std::string* __getName() const;
        void __freeName() const;

        int __id;
        const void* __userData;


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
        Object(const Object& src);
        Object(bool assignUniqueID = true);

    protected:
#ifdef OX_DEBUG
        unsigned int __check;
        void __doCheck();
#else
        void __doCheck() {}
#endif
    };


    template <class dest, class src>
    dest safeCast(src ptr)
    {
        if (!ptr)
            return 0;
#ifdef OXYGINE_DEBUG_SAFECAST
        dest cast = dynamic_cast<dest>(ptr);
        OX_ASSERT(cast && "can't cast pointer");
        return cast;
#else
        return static_cast<dest>(ptr);
#endif
    }

    template<class T, class U>
    intrusive_ptr<T> safeSpCast(intrusive_ptr<U> const& p)
    {
        if (!p)
            return 0;
#ifdef OXYGINE_DEBUG_SAFECAST
        intrusive_ptr<T> t = dynamic_cast<T*>(p.get());
        OX_ASSERT(t && "can't cast pointer");
#endif
        return static_cast<T*>(p.get());
    }
}

EDITOR_INCLUDE(Object);