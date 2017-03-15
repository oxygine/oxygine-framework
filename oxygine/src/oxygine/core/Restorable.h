#pragma once
#include "../oxygine-include.h"
#include "../closure/closure.h"
#include <vector>

namespace oxygine
{
    class Restorable
    {
    public:
        Restorable();
        virtual ~Restorable();

        typedef std::vector<Restorable*> restorable;
        static const restorable& getObjects();
        static void restoreAll();
        static void releaseAll();
        static bool isRestored();

        virtual void* _getRestorableObject() = 0;
        virtual void release() = 0;

        void restore();
        typedef Closure<void (Restorable*, void* userData)> RestoreCallback;

        void reg(RestoreCallback cb, void* user);
        void unreg();

    protected:

    private:
        //non copyable
        Restorable(const Restorable&);
        const Restorable& operator=(const Restorable&);

        RestoreCallback _cb;
        void* _userData;
        bool _registered;
    };
}