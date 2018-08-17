#include "oxygine-framework.h"
#include <functional>
#include <experimental/coroutine>
#include <variant>
using namespace oxygine;

//it is our resources
//in real project you would have more than one Resources declarations.
//It is important on mobile devices with limited memory and you would load/unload them
Resources gameResources;



namespace detail {
    // simple type erasure for iterators

    template<typename T>
    struct generic_iterable
    {
        virtual ~generic_iterable() = default;

        virtual T& operator*() = 0;
        virtual generic_iterable<T>& operator++() = 0;
        virtual bool empty() const = 0;

        bool await_ready() const noexcept
        {
            return empty();
        }

        template<typename U>
        void await_suspend(std::experimental::coroutine_handle<U> h) noexcept
        {
            h.promise().store_iterator(this);
        }

        void await_resume() const noexcept {}
    };

    template<typename T, typename StartIterator, typename EndIterator>
    struct iterator_iterable : public generic_iterable<T>
    {
        iterator_iterable(StartIterator start, EndIterator end) : start(start), end(end) {}

        T& operator*() override
        {
            return *start;
        }

        iterator_iterable<T, StartIterator, EndIterator>& operator++() override
        {
            ++start;
            return *this;
        }

        bool empty() const override
        {
            return start == end;
        }

        StartIterator start;
        EndIterator end;
    };

} // namespace detail

template<typename T>
struct generator
{
    using value_type = T;

    struct promise_type
    {
        // 0: prestart, 1: value, 2: range, 3: done
        std::variant<std::monostate, T*, detail::generic_iterable<T>*, std::monostate> state;

        promise_type& get_return_object() noexcept
        {
            return *this;
        }

        std::experimental::suspend_always initial_suspend() const noexcept
        {
            return {};
        }

        std::experimental::suspend_always final_suspend() const noexcept
        {
            return {};
        }

        std::experimental::suspend_always yield_value(T& value) noexcept
        {
            state.template emplace<1>(std::addressof(value));
            return {};
        }

        std::experimental::suspend_always yield_value(T&& value) noexcept
        {
            state.template emplace<1>(std::addressof(value));
            return {};
        }

        void return_void() noexcept
        {
            state.template emplace<3>();
        }

        template<typename Range>
        auto await_transform(Range&& range) const noexcept
        {
            using std::begin;
            using std::end;
            auto s = begin(range);
            auto e = end(range);

            // TODO: properly constraint
            static_assert(std::is_same_v<decltype(*s), T&>);
            detail::iterator_iterable<T, decltype(s), decltype(e)> iterator{ s, e };
            return iterator;
        }

        void unhandled_exception()
        {
            state.template emplace<3>();
            auto ex = std::current_exception();
            std::rethrow_exception(ex);
            //// MSVC bug? should be possible to rethrow with "throw;"
            //// rethrow exception immediately
            // throw;
        }

        void store_iterator(detail::generic_iterable<T>* iterator) noexcept
        {
            state.template emplace<2>(iterator);
        }

        T& value()
        {
            switch (state.index()) {
            case 1:
                return *std::get<1>(state);
            case 2:
                return **std::get<2>(state);
            case 0:
                next();
                return value();
            default:
            case 3:
                throw std::logic_error("Generator already completed!");
            }
        }

        const T& value() const
        {
            switch (state.index()) {
            case 1:
                return *std::get<1>(state);
            case 2:
                return **std::get<2>(state);
            case 0:
                next();
                return value();
            default:
            case 3:
                throw std::logic_error("Generator already completed!");
            }
        }

        void next()
        {
            auto handle = std::experimental::coroutine_handle<promise_type>::from_promise(*this);
            switch (state.index()) {
            case 0:
            case 1:
                handle.resume();
                break;
            case 2: {
                auto& iterator = *std::get<2>(state);
                ++iterator;
                if (iterator.empty()) {
                    state.template emplace<0>();
                    handle.resume();
                }
                break;
            }
            default:
            case 3:
                throw std::logic_error("Generator already completed!");
            }
        }
    };

    using handle_type = std::experimental::coroutine_handle<promise_type>;

    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T * ;
        using reference = T & ;

        handle_type coro_handle;

        iterator() : coro_handle(nullptr) {}

        iterator(handle_type coro_handle) : coro_handle(coro_handle) {}

        iterator& operator++()
        {
            try {
                coro_handle.promise().next();
            }
            catch (...) {
                coro_handle = nullptr;
                throw;
            }
            if (coro_handle.done())
                coro_handle = nullptr;
            return *this;
        }

        iterator operator++(int) = delete;

        bool operator==(iterator const& other) const
        {
            return coro_handle == other.coro_handle;
        }

        bool operator!=(iterator const& other) const
        {
            return !(*this == other);
        }

        const T& operator*() const
        {
            return coro_handle.promise().value();
        }

        const T* operator->() const
        {
            return std::addressof(operator*());
        }

        T& operator*()
        {
            return coro_handle.promise().value();
        }

        T* operator->()
        {
            return std::addressof(operator*());
        }
    };

    iterator begin()
    {
        if (coro_handle) {
            if (coro_handle.done())
                return {};
        }

        return { coro_handle };
    }

    iterator end()
    {
        return {};
    }

    generator(promise_type& promise) : coro_handle(handle_type::from_promise(promise)) {}

    generator() = default;
    generator(generator const&) = delete;
    generator& operator=(generator const&) = delete;

    generator(generator&& other) : coro_handle(other.coro_handle)
    {
        other.coro_handle = nullptr;
    }

    generator& operator=(generator&& other)
    {
        if (&other != this) {
            coro_handle = other.coro_handle;
            other.coro_handle = nullptr;
        }
        return *this;
    }

    ~generator()
    {
        if (coro_handle) {
            coro_handle.destroy();
        }
    }

private:
    std::experimental::coroutine_handle<promise_type> coro_handle = nullptr;
};



class MainActor: public Actor
{
public:
    spTextField _text;
    spSprite    _button;

    MainActor()
    {
        //create button Sprite
        spSprite button = new Sprite();


        //setup it:
        //set button.png image. Resource 'button' defined in 'res.xml'
        button->setResAnim(gameResources.getResAnim("button"));

        //centered button at screen
        Vector2 pos = getStage()->getSize() / 2 - button->getSize() / 2;
        button->setPosition(pos);

        //register  click handler to button
        EventCallback cb = CLOSURE(this, &MainActor::buttonClicked);
        button->addEventListener(TouchEvent::CLICK, cb);

        //animate mouse over and mouse out events
        cb = CLOSURE(this, &MainActor::buttonOverOut);
        button->addEventListener(TouchEvent::OVER, cb);
        button->addEventListener(TouchEvent::OUTX, cb);

#ifdef CLOSURE_FUNCTION //if your compiler supports lambda

        button->addEventListener(TouchEvent::CLICK, [](Event * e)->void
        {
            logs::messageln("button clicked");
        });

#endif

        //attach button as child to current actor
        addChild(button);
        _button = button;



        //create TextField Actor
        spTextField text = new TextField();
        //attach it as child to button
        text->attachTo(button);
        //centered in button
        text->setPosition(button->getSize() / 2);

        //initialize text style
        TextStyle style = TextStyle(gameResources.getResFont("main")).withColor(Color::White).alignMiddle();
        text->setStyle(style);
        text->setText("Click\nMe!");

        _text = text;
    }

    void buttonClicked(Event* event)
    {
        //user clicked to button

        //animate button by chaning color
        _button->setColor(Color::White);
        _button->addTween(Sprite::TweenColor(Color::Green), 500, 1, true);

        //animate text by scaling
        _text->setScale(1.0f);
        _text->addTween(Actor::TweenScale(1.1f), 500, 1, true);

        //and change text
        _text->setText("Clicked!");

        //lets create and run sprite with simple animation
        runSprite();
    }

    void buttonOverOut(Event* e)
    {
        if (e->type == TouchEvent::OVER)
        {
            _button->addTween(Sprite::TweenAddColor(Color(64, 64, 64, 0)), 300);
        }

        if (e->type == TouchEvent::OUTX)
        {
            _button->addTween(Sprite::TweenAddColor(Color(0, 0, 0, 0)), 300);
        }
    }

    void runSprite()
    {
        spSprite sprite = new Sprite();
        addChild(sprite);

        int duration = 600;//ms
        int loops = -1;//infinity loops

        //animation has 8 columns - frames, check 'res.xml'
        ResAnim* animation = gameResources.getResAnim("anim");

        //add animation tween to sprite
        //TweenAnim would change animation frames
        sprite->addTween(Sprite::TweenAnim(animation), duration, loops);

        Vector2 destPos = getStage()->getSize() - sprite->getSize();
        Vector2 srcPos = Vector2(0, destPos.y);
        //set sprite initial position
        sprite->setPosition(srcPos);

        //add another tween: TweenQueue
        //TweenQueue is a collection of tweens
        spTweenQueue tweenQueue = new TweenQueue();
        tweenQueue->setDelay(1500);
        //first, move sprite to dest position
        tweenQueue->add(Sprite::TweenPosition(destPos), 2500, 1);
        //then fade it out smoothly
        tweenQueue->add(Sprite::TweenAlpha(0), 500, 1);

        sprite->addTween(tweenQueue);

        //and remove sprite from tree when tweenQueue is empty
        //if you don't hold any references to sprite it would be deleted automatically
        tweenQueue->detachWhenDone();
    }
};
//declare spMainActor as intrusive_ptr holder of MainActor
typedef oxygine::intrusive_ptr<MainActor> spMainActor;
//you could use DECLARE_SMART preprocessor definition it does the same:
//DECLARE_SMART(MainActor, spMainActor)

void example_preinit() {}


generator<int> test()
{
    logs::messageln("a");
    co_yield 12;
    logs::messageln("b");
    co_yield 13;
    logs::messageln("c");
    co_yield 14;
}

//called from main.cpp
void example_init()
{
    for (auto q : test())
    {
        //int q = 0;
    }

    //load xml file with resources definition
    gameResources.loadXML("res.xml");


    //lets create our client code simple actor
    //spMainActor was defined above as smart intrusive pointer (read more: http://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/intrusive_ptr.html)
    spMainActor actor = new MainActor;

    //and add it to Stage as child
    getStage()->addChild(actor);
}


//called each frame from main.cpp
void example_update()
{
}

//called each frame from main.cpp
void example_destroy()
{
    //free previously loaded resources
    gameResources.free();
}
