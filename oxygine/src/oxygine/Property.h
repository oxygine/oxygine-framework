#pragma once
namespace oxygine
{
    template<typename Value, typename getValueRef, typename setValueRef, typename C, getValueRef(C::*GetF)() const, void (C::*SetF)(setValueRef)>
    class Property0
    {
    public:
        typedef C type;
        typedef Value value;

        Property0(getValueRef dest) : _dest(dest), _initialized(false) {}

        void init(type& t)
        {
            _initialized = true;
            _src = get(t);
        }

        void init(getValueRef src)
        {
            _initialized = true;
            _src = src;
        }

        void setSrc(type& t)
        {
            set(t, _src);
        }

        void setDest(type& t)
        {
            set(t, _dest);
        }

        void update(type& t, float p, const UpdateState& us)
        {
            OX_ASSERT(_initialized);
            value v = lerp(_src, _dest, p);
            set(t, v);
        }

        void done(type& t)
        {
        }

        static getValueRef get(C& c)
        {
            return (c.*GetF)();
        }

    private:
        value _dest;
        value _src;
        bool _initialized;

        static void set(C& c, setValueRef v)
        {
            return (c.*SetF)(v);
        }
    };

    template<typename Value, typename valueRef, typename C, valueRef(C::*GetF)() const, void (C::*SetF)(valueRef)>
    class Property: public Property0<Value, valueRef, valueRef, C, GetF, SetF>
    {
    public:
        Property(valueRef v): Property0<Value, valueRef, valueRef, C, GetF, SetF>(v) {}
    };

    template<typename value0, typename value, typename valueRef, typename C, valueRef(C::*GetF)() const, void (C::*SetF)(valueRef)>
    class Property2Args : public Property < value, valueRef, C, GetF, SetF >
    {
        typedef Property<value, valueRef, C, GetF, SetF> GS;
    public:
        Property2Args(value0 v1, value0 v2) : GS(value(v1, v2)) {}
        Property2Args(valueRef v) : GS(v) {}
    };


    template<typename value0, typename value, typename getValueRef, typename setValueRef, typename C, getValueRef(C::*GetF)() const, void (C::*SetF)(setValueRef)>
    class Property2Args2 : public Property0 < value, getValueRef, setValueRef, C, GetF, SetF >
    {
        typedef Property0<value, getValueRef, setValueRef, C, GetF, SetF> GS;
    public:
        Property2Args2(value0 v1, value0 v2) : GS(value(v1, v2)) {}
        Property2Args2(getValueRef v) : GS(v) {}
    };

    template<typename value0, typename value, typename valueRef, typename C, valueRef(C::*GetF)() const, void (C::*SetF)(valueRef)>
    class Property2Args1Arg : public Property < value, valueRef, C, GetF, SetF >
    {
        typedef Property<value, valueRef, C, GetF, SetF> GS;
    public:
        Property2Args1Arg(value0 v) : GS(value(v, v)) {}
        Property2Args1Arg(value0 v1, value0 v2) : GS(value(v1, v2)) {}
        Property2Args1Arg(valueRef v) : GS(v) {}
    };

    //backward compatibility
#define GetSet          Property
#define GetSet2Args     Property2Args
#define GetSet2Args1Arg Property2Args1Arg

}
