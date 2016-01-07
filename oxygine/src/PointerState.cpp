#include "PointerState.h"
#include "Actor.h"
namespace oxygine
{
    PointerState::PointerState(): _index(0)
    {
        for (int i = 0; i < MouseButton_Count; ++i)
            _isPressed[i] = false;
    }

    void PointerState::init(int ID)
    {
        _index = ID;
    }

    bool isFriend22(Actor* actor, Actor* max_parent, Actor* checkParent)
    {
        if (!max_parent)
            max_parent = actor;

        Actor* parent = actor;
        while (parent)
        {
            if (parent == checkParent)
                return true;
            Actor* copy = parent;
            parent = parent->getParent();

            if (copy == max_parent)
                break;
        }
        return false;
    }
    /*
    bool checkParent(spEventHandler eh, Actor *checkIsItChild)
    {
        Actor *parent = eh->getFriendActor();
        if (!parent)
        {
            if (eh->getClient() == checkIsItChild)
                return true;
            return false;
        }
        while (checkIsItChild)
        {
            if (checkIsItChild == parent)
                return true;
            checkIsItChild = checkIsItChild->getParent();
        }
        return false;
    }
    */

    Vector2 global2local(Actor* actor, const Vector2& globalPos)
    {
        Vector2 pos = globalPos;
        if (!actor)
            return pos;
        Actor* parent = actor->getParent();
        if (parent)
        {
            pos = global2local(parent, globalPos);
        }

        pos = actor->global2local(pos);

        return pos;
    }

}