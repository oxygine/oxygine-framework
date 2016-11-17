#pragma once
#include "oxygine-include.h"
#include "math/Vector2.h"

namespace oxygine
{
    enum VertexFormatFlags
    {
        VERTEX_POSITION = 0x0001,
        VERTEX_COLOR    = 0x0002,

        VERTEX_LAST_BIT = 8,

        USER_DATA_SIZE_BIT = VERTEX_LAST_BIT + 4,
        NUM_TEX_COORD_BIT = VERTEX_LAST_BIT + 7,
        TEX_COORD_SIZE_BIT = VERTEX_LAST_BIT + 10,

        USER_DATA_SIZE_MASK = (0x7 << USER_DATA_SIZE_BIT),
        NUM_TEX_COORD_MASK = (0x7 << NUM_TEX_COORD_BIT),
        TEX_COORD_SIZE_MASK = (0x7 << TEX_COORD_SIZE_BIT),
    };

#define VERTEX_USERDATA_SIZE(_n)            ((_n) << USER_DATA_SIZE_BIT)
#define VERTEX_NUM_TEXCOORDS(_n)            ((_n) << NUM_TEX_COORD_BIT)
#define VERTEX_TEXCOORD_SIZE(_stage, _n)    ((_n) << (TEX_COORD_SIZE_BIT + (3*_stage)))

    typedef unsigned int bvertex_format;

    inline int vertexFlags(bvertex_format vertexFormat)
    {
        return vertexFormat & ((1 << (VERTEX_LAST_BIT + 1)) - 1);
    }

    inline int numTextureCoordinates(bvertex_format vertexFormat)
    {
        return (vertexFormat >> NUM_TEX_COORD_BIT) & 0x7;
    }

    inline int userDataSize(bvertex_format vertexFormat)
    {
        return (vertexFormat >> USER_DATA_SIZE_BIT) & 0x7;
    }

    inline int texCoordSize(int stage, bvertex_format vertexFormat)
    {
        return (vertexFormat >> (TEX_COORD_SIZE_BIT + 3 * stage)) & 0x7;
    }



    inline unsigned int getVertexSize(bvertex_format fmt)
    {
        int offset = 0;

        if (fmt & VERTEX_POSITION)
        {
            offset += sizeof(float) * 3;
        }

        if (fmt & VERTEX_COLOR)
        {
            offset += 4;
        }

        int numTexCoords = numTextureCoordinates(fmt);
        for (int j = 0; j < numTexCoords; ++j)
        {
            int coordSize = texCoordSize(j, fmt);
            offset += sizeof(float) * coordSize;
        }

        int ds = userDataSize(fmt);
        if (ds > 0)
        {
            offset += sizeof(float) * ds;
        }
        return offset;
    }

    struct vertexPT2
    {
        enum { FORMAT = VERTEX_POSITION | VERTEX_NUM_TEXCOORDS(1) | VERTEX_TEXCOORD_SIZE(0, 2) };
        float x, y, z;
        float u, v;
    };

    struct vertexP2C
    {
        float x, y;
        unsigned int color;

        Vector2& getPos() {return *((Vector2*)&x);}
    };

    struct vertexPCT2
    {
        enum { FORMAT = VERTEX_POSITION | VERTEX_COLOR | VERTEX_NUM_TEXCOORDS(1) | VERTEX_TEXCOORD_SIZE(0, 2) };
        float x, y, z;
        unsigned int color;
        float u, v;
    };

    struct vertexPCT2T2 : public vertexPCT2
    {
        enum { FORMAT = VERTEX_POSITION | VERTEX_COLOR | VERTEX_NUM_TEXCOORDS(2) | VERTEX_TEXCOORD_SIZE(0, 2) | VERTEX_TEXCOORD_SIZE(1, 2) };
        float u2, v2;
    };

    struct vertexPCT2T2T2 : public vertexPCT2T2
    {
        enum { FORMAT = VERTEX_POSITION | VERTEX_COLOR | VERTEX_NUM_TEXCOORDS(3) | VERTEX_TEXCOORD_SIZE(0, 2) | VERTEX_TEXCOORD_SIZE(1, 2) | VERTEX_TEXCOORD_SIZE(2, 2) };
        float u3, v3;
    };

#define VERTEX_PCT2 vertexPCT2::FORMAT
#define VERTEX_PCT2T2 vertexPCT2T2::FORMAT

}
