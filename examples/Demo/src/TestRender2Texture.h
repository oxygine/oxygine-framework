#pragma once
#include "test.h"

class TestRender2Texture: public Test
{
public:
	spNativeTexture texture;
	Color color;
	spSprite preview;

	TestRender2Texture()
	{
		notify("touch to draw", 100000);

		color = Color(255,255,255,32);

		Vector2 size = content->getSize();

		texture = IVideoDriver::instance->createTexture();
		texture->init((int)size.x, (int)size.y, TF_R8G8B8A8, true);

		preview = new Sprite;
		preview->attachTo(content);
		

		AnimationFrame frame;
		Diffuse df;
		df.base = texture;
		frame.init(0, df,
			RectF(0, 0, size.x/texture->getWidth(), size.y/texture->getHeight()), 
			RectF(Vector2(0,0), size), size);
		preview->setAnimFrame(frame);		
		preview->setBlendMode(blend_disabled);

		content->addEventListener(TouchEvent::MOVE, CLOSURE(this, &TestRender2Texture::onMove));
		content->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &TestRender2Texture::onDown));
	}

	void onDown(Event *ev)
	{
		color = Color(rand() % 255, rand() % 255, rand() % 255, 32);
		onMove(ev);
	}

	void onMove(Event *ev)
	{
		TouchEvent *te = (TouchEvent *)ev;
		if (!te->getPointer()->isPressed())
			return;

		Renderer r;
		RenderState rs;
		rs.renderer = &r;


		Point size = content->getSize().cast<Point>();

		r.initCoordinateSystem(size.x, size.y, true);
		
		Rect vp(Point(0, 0), size);

		r.begin(texture, vp, 0);
		ResAnim *brush = resources.getResAnim("brush");
		AnimationFrame frame = brush->getFrame(0,0);
		const Diffuse &df = frame.getDiffuse();
		r.setDiffuse(df);
		r.setPrimaryColor(color);
		r.setBlendMode(blend_alpha);
		float pressure =  te->pressure;
		//log::messageln("pressure %.2f", pressure);
		//pressure = pressure * pressure;
		r.draw(frame.getSrcRect(), 
			RectF(te->localPosition - Vector2(16, 16) * pressure, Vector2(32, 32)  * pressure));
		r.end();
	}
};
