#pragma once
#include "../oxygine-include.h"
namespace oxygine
{
    template<class T>
    class intrusive_list
    {
    public:
        T _first;
        T _last;

        ~intrusive_list()
        {
            clear();
        }

        bool empty() const
        {
            return _first == 0;
        }

        void clear()
        {
            T item = _first;
            while (item)
            {
                T copy = item;
                item = item->_next;
                if (copy)
                {
                    copy->_list = 0;
                    copy->_next = 0;
                    copy->_prev = 0;
                }
            }
            _first = 0;
            _last = 0;
        }

        void insert_after(T& child, T& after)
        {
            OX_ASSERT(after->_list == this);
            child->_list = this;

            if (after->_next)
                after->_next->_prev = child;
            else
                _last = child;

            child->_next = after->_next;
            child->_prev = after;
            after->_next = child;


            checkList();
        }

        void insert_before(T& child, T& before)
        {
            child->_list = this;

            if (before->_prev)
                before->_prev->_next = child;
            else
                _first = child;

            child->_prev = before->_prev;
            child->_next = before;
            before->_prev = child;


            checkList();
        }

        void append(T& child)
        {
            child->_list = this;

            if (_first)
            {
                _last->_next = child;
                child->_prev = _last;
            }
            else
            {
                _first = child;
                child->_prev = 0;
            }

            _last = child;
            child->_next = 0;

            checkList();
        }

        void prepend(T& child)
        {
            child->_list = this;

            if (_first)
            {
                _first->_prev = child;
                child->_next = _first;
            }
            else
            {
                _last = child;
                child->_next = 0;
            }

            _first = child;
            child->_prev = 0;

            checkList();
        }

        void checkList()
        {
            return;
            if (!_first)
                return;

            T v = _first;
            do
            {
                T p = v;
                v = v->_next;

                if (v)
                {
                    OX_ASSERT(v->_prev == p);
                }
                else
                {
                    OX_ASSERT(p == _last);
                }

                //OX_ASSERT(v == _last && v->_next == 0);
            }
            while (v);

        }



        void remove(T& child)
        {
            OX_ASSERT(child->_list == this);
            child->_list = 0;

            if (child->_next)
            {
                OX_ASSERT(child->_next->_prev == child);
                child->_next->_prev = child->_prev;
            }
            else
            {
                OX_ASSERT(child == _last);
                _last = child->_prev;
            }


            if (child->_prev)
            {
                OX_ASSERT(child->_prev->_next == child);
                child->_prev->_next = child->_next;
            }
            else
            {
                OX_ASSERT(child == _first);
                _first = child->_next;
            }

            child->_prev = 0;
            child->_next = 0;

            checkList();
        }
    };


    template<class T>
    class intrusive_list_item
    {
    public:

        intrusive_list<T>* getParentList() {return _list;}

    protected:
        T& getNextSibling() {return _next;}
        T& getPrevSibling() {return _prev;}


        intrusive_list_item(): _prev(T(0)), _next(T(0)), _list(0) {}
        T _prev;
        T _next;

        intrusive_list<T>* _list;

        template<class U>
        friend class intrusive_list;
    };
}