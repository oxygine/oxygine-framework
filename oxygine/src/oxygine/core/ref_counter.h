#pragma once
#include "../oxygine-include.h"
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


    class AutoRefHolder
    {
    public:
        AutoRefHolder(ref_counter* rc) : _rc(rc)
        {
            _rc->addRef();
        }

        ~AutoRefHolder()
        {
            _rc->releaseRef();
        }

        ref_counter* _rc;

    private:
        AutoRefHolder(const AutoRefHolder&);
        AutoRefHolder& operator=(const AutoRefHolder&);
    };
}
