#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "Sprite.h"
#include "Button.h"
#include "TextField.h"
namespace oxygine
{

	namespace args
	{
		template<typename value, typename valueRef, typename C, void (C::*SetF)(valueRef)>
		class argT
		{
			bool _init;
			value _v;

		public:				
			argT():_init(false){}

			argT &operator = (valueRef v)
			{
				_init = true;
				_v = v;
				return *this;
			}

			argT &operator = (intrusive_ptr<C> v)
			{
				_init = true;
				_v = v.get();
				return *this;
			}

			void apply(C *c) const
			{
				if (!_init)
					return;
				return (c->*SetF)(_v);
			}
		};

		typedef argT<Vector2, const Vector2 &, Actor, &Actor::setAnchor> anchor;
		typedef argT<Vector2, const Vector2 &, Actor, &Actor::setPosition> position;
		typedef argT<float, float, Actor, &Actor::setX> x;
		typedef argT<float, float, Actor, &Actor::setY> y;
		typedef argT<float, float, Actor, &Actor::setRotation> rotation;
		typedef argT<bool, bool, Actor, &Actor::setVisible> visible;
		typedef argT<float, float, Actor, &Actor::setWidth> width;
		typedef argT<float, float, Actor, &Actor::setHeight> height;
		typedef argT<Vector2, const Vector2 &, Actor, &Actor::setSize> size;
		typedef argT<float, float, Actor, &Actor::setScale> scale;
		typedef argT<Vector2, const Vector2 &, Actor, &Actor::setScale> scale2;	
		typedef argT<std::string, const std::string &, ObjectBase, &ObjectBase::setName> name;
		typedef argT<float, float, Actor, &Actor::setScaleX> scaleX;
		typedef argT<float, float, Actor, &Actor::setScaleY> scaleY;
		typedef argT<unsigned char, unsigned char, Actor, &Actor::setAlpha> alpha;	
		typedef argT<bool, bool, Actor, &Actor::setInputEnabled> input;
		typedef argT<Actor*, Actor*, Actor, &Actor::attachTo> attachTo;
		typedef argT<Color, const Color &, VStyleActor, &VStyleActor::setColor> color;
		typedef argT<blend_mode, blend_mode, VStyleActor, &VStyleActor::setBlendMode> blend;
		typedef argT<short, short, Actor, &Actor::setPriority> priority;
		typedef argT<void*, void*, ObjectBase, &ObjectBase::setUserData> userData;
		typedef argT<char, char, Actor, &Actor::setExtendedClickArea> extendedClickArea;

		typedef argT<TextStyle, const TextStyle &, TextField, &TextField::setStyle> style;	
		typedef argT<std::string, const std::string &, TextField, &TextField::setText> text;	
		typedef argT<std::string, const std::string &, TextField, &TextField::setHtmlText> htmlText;	
		typedef argT<TextStyle::HorizontalAlign, TextStyle::HorizontalAlign, TextField, &TextField::setHAlign> hAlign;	
		typedef argT<TextStyle::VerticalAlign, TextStyle::VerticalAlign, TextField, &TextField::setVAlign> vAlign;	
		typedef argT<int, int, TextField, &TextField::setFontSize2Scale> fontSize2Scale;	
		typedef argT<Font*, Font*, TextField, &TextField::setFont> font;	
		typedef argT<bool, bool, TextField, &TextField::setMultiline> multiline;


		class animFrame
		{
			const ResAnim *_resAnim;
			int _col;
			int _row;
			bool _managed;
			AnimationFrame _frame;
			bool _useResAnim;

		public:
			animFrame(bool managed, int col, int row):_col(col), _row(row), _resAnim(0), _useResAnim(true), _managed(managed){}

			animFrame &operator = (const ResAnim *r)
			{
				_useResAnim = true;
				_resAnim = r;
				return *this;
			}

			animFrame &operator = (const AnimationFrame &frame)
			{
				_useResAnim = false;
				_frame = frame;
				return *this;
			}

			void apply(Sprite *s) const
			{
				if (_managed)
					s->setManageResAnim(_managed);
				if (_useResAnim)
					return s->setAnimFrame(_resAnim, _col, _row);
				return s->setAnimFrame(_frame);
			}

			void apply(Button *b) const
			{
				OX_ASSERT(_useResAnim);
				b->setResAnim(_resAnim);
			}
		};


#define arg_position args::position()
#define arg_pos arg_position
#define arg_x args::x()
#define arg_y args::y()

#define arg_anchor args::anchor()

#define arg_rotation args::rotation()
#define arg_rot arg_rotation

#define arg_visible args::visible()
#define arg_vis args::visible()

#define arg_width args::width()
#define arg_w arg_width
#define arg_height args::height()
#define arg_h arg_height
#define arg_size args::size()

#define arg_scale args::scale()
#define arg_scale2 args::scale2()
#define arg_scaleX args::scaleX()
#define arg_scaleY args::scaleY()

#define arg_name args::name()

#define arg_alpha args::alpha()
#define arg_color args::color()
#define arg_blend args::blend()

#define arg_input args::input()
#define arg_attachTo args::attachTo()
#define arg_priority args::priority()
#define arg_userData args::userData()

#define arg_extendedClickArea args::extendedClickArea()
		
		

//sprite or button
#define arg_animFrame args::animFrame(false, 0, 0)
#define arg_resAnim args::animFrame(false, 0, 0)
#define arg_managedResAnim args::animFrame(true, 0, 0)

//TextField
#define arg_style args::style()
#define arg_text args::text()
#define arg_htmlText args::htmlText()
#define arg_hAlign args::hAlign()
#define arg_vAlign args::vAlign()
#define arg_fontSize2Scale args::fontSize2Scale()
#define arg_font args::font()
#define arg_multiline args::multiline()
#define arg_tcolor args::color()
//Button
#define arg_onClick args::onClick()


	}

	template<class T>
	T initActor(T actor)
	{
		return actor;
	}

	template<class T, class A1>
	T initActor(T actor, const A1 &arg1)
	{
		initActor(actor);
		arg1.apply(actor);

		return actor;
	}

	template<class T, class A1, class A2>
	T initActor(T actor, const A1 &arg1, const A2 &arg2)
	{
		initActor(actor, arg1);
		arg2.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3)
	{
		initActor(actor, arg1, arg2);
		arg3.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3, class A4>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3, const A4 &arg4)
	{
		initActor(actor, arg1, arg2, arg3);
		arg4.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3, class A4, class A5>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3, const A4 &arg4, const A5 &arg5)
	{
		initActor(actor, arg1, arg2, arg3, arg4);
		arg5.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3, class A4, class A5, class A6>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3, const A4 &arg4, const A5 &arg5, const A6 &arg6)
	{
		initActor(actor, arg1, arg2, arg3, arg4, arg5);
		arg6.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3, const A4 &arg4, const A5 &arg5, const A6 &arg6, const A7 &arg7)
	{
		initActor(actor, arg1, arg2, arg3, arg4, arg5, arg6);
		arg7.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3, const A4 &arg4, const A5 &arg5, const A6 &arg6, const A7 &arg7, const A8 &arg8)
	{
		initActor(actor, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		arg8.apply(actor);
		return actor;
	}

	template<class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	T initActor(T actor, const A1 &arg1, const A2 &arg2, const A3 &arg3, const A4 &arg4, const A5 &arg5, const A6 &arg6, const A7 &arg7, const A8 &arg8, const A9 &arg9)
	{
		initActor(actor, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
		arg9.apply(actor);
		return actor;
	}
}