#pragma once
#include "oxygine-include.h"

#include "intrusive_ptr.h"

namespace oxygine
{
    /** RefCounter **/
    class ref_counter
    {
    public:
        int _ref_counter;

        ref_counter(): _ref_counter(0) {}
        virtual ~ref_counter() {}

        void addRef()
        {
            ++_ref_counter;
        }

        void releaseRef()
        {
            if (0 == --_ref_counter)
                delete this;
        }


        //commented, unsafe
        //todo, find solution

    private:
        ref_counter(const ref_counter&);
        const ref_counter& operator=(const ref_counter&);

    };


    inline void intrusive_ptr_add_ref(ref_counter* p)
    {
        p->addRef();
    }

    inline void intrusive_ptr_release(ref_counter* p)
    {
        p->releaseRef();
    }
}

#define DECLARE_SMART(class_name, spname) \
    class class_name;\
    typedef oxygine::intrusive_ptr<class_name> spname;

#define DECLARENS_SMART(name_space, class_name, spname) \
    namespace name_space \
    {\
        class class_name;\
        typedef oxygine::intrusive_ptr<class_name> spname;\
    }
