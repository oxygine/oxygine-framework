#pragma once
#include "test.h"
#include "Polygon.h"
#include "res/ResAnim.h"
#include "core/NativeTexture.h"
#include "AnimationFrame.h"

class TestPolygon: public Test
{
public:
	spPolygon poly;
	int _num;

	TestPolygon() :_num(3)
	{	
		poly = new oxygine::Polygon;
		poly->addTween(Actor::TweenRotation(MATH_PI * 2), 16000, -1);
		poly->setPosition(content->getSize() / 2);

		ResAnim *rs = resources.getResAnim("tiled");

		//access to texture
		//ResAnim "tiled" has only single frame and uses own separate atlas texture
		AnimationFrame frame = rs->getFrame(0);
		spNativeTexture texture = frame.getDiffuse().base;

		//by default all textures has mode "clamp to edge"
		//disable it and allow tiling
		texture->setWrapMode(false);

		poly->setResAnim(rs);

		update(_num);

		content->addChild(poly);

		addButton("+1", "+1");
	}

	static vertexPCT2 initVertex(const Vector2 &pos, unsigned int color)
	{
		vertexPCT2 v;
		v.color = color;
		v.x = pos.x;
		v.y = pos.y;
		v.z = 0;
		v.u = v.x / 100;
		v.v = v.y / 100;

		return v;
	}

	static vertexPCT2 getVertex(int i, int num)
	{
		float theta = 2.0f * MATH_PI / num * i;

		Vector2 dir = Vector2(scalar::cos(theta), scalar::sin(theta));
		float rad = getRoot()->getHeight() * 0.4f * (1 + scalar::sin(theta * 10)/10);
		Vector2 p = dir * rad;
		
		Color c = interpolate(Color(Color::Lime), Color(Color::Red), scalar::abs(scalar::sin(theta * 3)));
		return initVertex(p, c.rgba());
	}

	static vertexPCT2 *createVertices(int num)
	{
		int verticesCount = num * 4;

		vertexPCT2 * vertices = new vertexPCT2[verticesCount];

		vertexPCT2 *p = vertices;
		for (int n = 0; n < num; ++n)
		{
			//add centered vertex
			*p = initVertex(Vector2(0, 0), Color::White);
			++p;

			*p = getVertex(n, num);
			++p;

			*p = getVertex(n + 1, num);
			++p;

			//Oxygine uses "triangles strip" rendering mode 
			//dublicate last vertex (degenerate triangles)
			*p = getVertex(n + 1, num);
			++p;
		}

		return vertices;
	}

	void update(int num)
	{
		vertexPCT2 *vertices = createVertices(num);
		poly->setVertices(vertices, sizeof(vertexPCT2) * num * 4, vertexPCT2::FORMAT, true);
	}

	void clicked(string id)
	{
		_num++;
		update(_num);
	}
};
