#include "VertexDeclarationGL.h"
#include "../../utils/stringUtils.h"
namespace oxygine
{
    void VertexDeclarationGL::init(bvertex_format fmt)
    {
        bformat = fmt;
        int i = 0;
        int offset = 0;
        Element* dest = elements;
        if (fmt & VERTEX_POSITION)
        {
            strcpy(dest->name, "position");
            dest->elemType = GL_FLOAT;
            dest->size = 3;
            dest->offset = offset;
            dest->index = i;
            dest->normalized = false;

            offset += sizeof(float) * 3;
            ++i;
            ++dest;
        }

        if (fmt & VERTEX_COLOR)
        {
            strcpy(dest->name, "color");
            dest->elemType = GL_UNSIGNED_BYTE;
            dest->size = 4;
            dest->offset = offset;
            dest->index = i;
            dest->normalized = true;

            offset += 4;
            ++i;
            ++dest;
        }

        int numTexCoords = numTextureCoordinates(fmt);
        for (int j = 0; j < numTexCoords; ++j)
        {
            int coordSize = texCoordSize(j, fmt);
            if (j == 0)
                strcpy(dest->name, "uv");
            else
                safe_sprintf(dest->name, "uv%d", j + 1);

            dest->elemType = GL_FLOAT;
            dest->size = coordSize;
            dest->offset = offset;
            dest->index = i;
            dest->normalized = true;

            offset += sizeof(float) * coordSize;
            ++i;
            ++dest;
        }

        int userSize = userDataSize(fmt);
        if (userSize > 0)
        {
            dest->elemType = GL_FLOAT;
            dest->size = userSize;
            dest->offset = offset;
            dest->index = i;
            dest->normalized = false;

            offset += sizeof(float) * userSize;
            ++i;
            ++dest;
        }

        numElements = i;
        size = offset;
    }
}