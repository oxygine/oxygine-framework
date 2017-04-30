#pragma once
#include "../oxygine-include.h"
#include "Sprite.h"

namespace oxygine
{
    DECLARE_SMART(Polygon, spPolygon);
    class Polygon : public Sprite
    {
        INHERITED(Sprite);
    public:
        DECLARE_COPYCLONE_NEW(Polygon);

        Polygon();
        ~Polygon();

        /**
        if *own* is true Polygon will delete[] data array;
        */
        void setVertices(const void* data, int size, int bformat, bool own);

        void serialize(serializedata* data) override;
        void deserialize(const deserializedata* data) override;

        std::string dump(const dumpOptions&) const override;

    protected:
        void doRender(const RenderState&) override;

        const VertexDeclaration* _vdecl;
        bool _own;
        const unsigned char* _verticesData;
        int _verticesSize;
    };
}

EDITOR_INCLUDE(Polygon);