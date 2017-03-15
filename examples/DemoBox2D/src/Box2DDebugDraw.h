#pragma once
#include "oxygine-framework.h"
#include "Box2D/Box2D.h"

using namespace oxygine;
namespace oxygine
{
    class ShaderProgramGL;
}

DECLARE_SMART(Box2DDraw, spBox2DDraw);

class Box2DDraw: public Actor, public b2Draw
{
public:
    Box2DDraw();
    ~Box2DDraw();

    void setWorld(float worldScale, b2World* world) {_worldScale = worldScale; _world = world;}

    /// Draw a closed polygon provided in CCW order.
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    /// Draw a solid closed polygon provided in CCW order.
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    /// Draw a circle.
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

    /// Draw a solid circle.
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

    /// Draw a line segment.
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

    /// Draw a transform. Choose your own length scale.
    /// @param xf a transform.
    void DrawTransform(const b2Transform& xf);

protected:
    b2World* _world;
    float _worldScale;

    void doRender(const RenderState& rs);
    static const int MAX_VERTICES = 64;
    static const int CIRCLE_SEGMENTS = 16;

    Vector2 mVertices[MAX_VERTICES];
    void createCircleVertices(const b2Vec2& aCenter, float32 aRadius);
    void createPolygonVertices(const b2Vec2* aVertices, int32 aVertexCount);
    void drawPrimitives(bool drawTriangles, bool drawLines, int aCount, const b2Color& aColor);

    ShaderProgramGL* _program;
};
