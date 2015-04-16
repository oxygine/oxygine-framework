#include "test.h"
#include "oxygine-framework.h"

class Toggle: public Button
{
public:
    Toggle(const Test::toggle* t, int num): _current(0)
    {
        _items.assign(t, t + num);
    }

    int _current;

    std::vector<Test::toggle> _items;
};

spTextField createText(std::string txt)
{
    spTextField text = new TextField();

    TextStyle style;
    style.font = resourcesUI.getResFont("main")->getFont();
    style.color = Color(72, 61, 139, 255);
    style.vAlign = TextStyle::VALIGN_MIDDLE;
    style.hAlign = TextStyle::HALIGN_CENTER;
    style.multiline = true;

    text->setStyle(style);
    text->setText(txt.c_str());

    return text;
}

spButton createButtonHelper(spButton button, string txt, EventCallback cb)
{
    button->setPriority(10);
    //button->setName(id);
    button->setResAnim(resourcesUI.getResAnim("button"));
    button->addEventListener(TouchEvent::CLICK, cb);

    //create Actor with Text and it to button as child
    spTextField text = createText(txt);
    text->setSize(button->getSize());
    text->attachTo(button);

    return button;
}


Test::Test()
{
    setSize(getStage()->getSize());

    _x = getWidth() - 100;
    _y = 2;

    ui = new Actor;
    content = new Content;
    content->setSize(getSize());

    addChild(content);
    addChild(ui);

    if (_tests)
    {
        spButton button = createButtonHelper(new Button, "back", CLOSURE(this, &Test::back));
        button->setY(getHeight() - button->getHeight());
        ui->addChild(button);
    }

    memset(_notifies, 0, sizeof(_notifies));
}


Test::~Test()
{
}


spButton Test::addButton(std::string id, std::string txt)
{
    spButton button = createButtonHelper(new Button, txt, CLOSURE(this, &Test::_clicked));
    initActor(button.get(),
              arg_name = id,
              arg_attachTo = ui,
              arg_anchor = Vector2(0.5f, 0.0f),
              arg_pos = Vector2(_x, _y));

    _y += button->getHeight() + 2.0f;

    if (_y + button->getHeight() >= getHeight())
    {
        _y = 0;
        _x += button->getWidth() + 70;
    }

    return button;
}

void Test::addToggle(std::string id, const toggle* t, int num)
{
    spButton button = createButtonHelper(new Toggle(t, num), t[0].text, CLOSURE(this, &Test::_toggleClicked));
    initActor(button.get(),
              arg_name = id,
              arg_attachTo = ui,
              arg_anchor = Vector2(0.5f, 0.0f),
              arg_pos = Vector2(_x, _y));

    _y += button->getHeight() + 2.0f;

    if (_y + button->getHeight() >= getHeight())
    {
        _y = 0;
        _x += button->getWidth() + 70;
    }
}

void Test::updateText(std::string id, std::string txt)
{
    spActor child = ui->getChild(id);
    if (!child)
        return;

    spTextField t = safeSpCast<TextField>(child->getFirstChild());
    if (!t)
        return;
    t->setText(txt);
}


void Test::_clicked(Event* event)
{
    clicked(event->currentTarget->getName());
}

void Test::_toggleClicked(Event* event)
{
    Toggle* t = safeCast<Toggle*>(event->currentTarget.get());

    toggleClicked(event->currentTarget->getName(), &t->_items[t->_current]);

    t->_current = (t->_current + 1) % t->_items.size();
    spTextField ta = safeSpCast<TextField>(t->getFirstChild());
    const toggle* data = &t->_items[t->_current];
    ta->setText(data->text);
}


void Test::back(Event* event)
{
    detach();
    _tests->setVisible(true);
}


void Test::notify(std::string txt, int time)
{
    size_t N = 0;
    for (size_t i = 0; i < MAX_NOTIFIES; ++i)
    {
        if (_notifies[i])
            continue;
        N = i;
        break;
    }

    _notifies[N] += 1;


    spColorRectSprite sprite = new ColorRectSprite();
    sprite->setUserData((void*)N);
    sprite->setPriority(10);
    Color colors[] = {Color(0xD2691EFF), Color(0x7FFFD4FF), Color(0xDC143CFF), Color(0xADFF2FFF), };
    Color c = colors[rand() % 4];
    sprite->setColor(c);
    sprite->setSize(100, 30);
    //sprite->setAnimFrame(resourcesUI.getResAnim("button"));
    sprite->setAlpha(0);

    spTweenQueue tq = new TweenQueue;
    tq->add(Actor::TweenAlpha(255), 300, 1, false, 0, Tween::ease_inExpo);
    tq->add(Actor::TweenAlpha(0), 300, 1, false, 1200);
    tq->setDetachActor(true);
    tq->addDoneCallback(CLOSURE(this, &Test::notifyDone));

    sprite->addTween(tq);
    sprite->attachTo(ui);
    sprite->setPosition(2.0f, getHeight() - 100.0f - N * sprite->getHeight() * 1.1f);

    spTextField text = createText(txt);
    text->attachTo(sprite);
    text->setColor(Color::Black);
    text->setPosition(sprite->getSize() / 2);
}

void Test::notifyDone(Event* ev)
{
    size_t N = size_t(ev->target->getUserData());
    _notifies[N] -= 1;
}