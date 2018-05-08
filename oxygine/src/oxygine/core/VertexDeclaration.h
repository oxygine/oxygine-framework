#pragma once
#include "../oxygine-include.h"
#include "vertex.h"

namespace oxygine
{
    class VertexDeclaration
    {
    public:
        VertexDeclaration(): bformat(0), numElements(0), size(0) {}

        bvertex_format bformat;
        int numElements;
        int size;
    };

    template <class T>
    class VertexDeclarations
    {
    public:
        const T* get(bvertex_format bformat)
        {
            for (int i = 0; i < NUM; ++i)
            {
                T& decl = _declarations[i];
                if (decl.bformat == 0)
                    decl.init(bformat);

                if (decl.bformat == bformat)
                    return &decl;
            }

            return 0;
        }

    protected:
        enum {NUM = 8};
        T _declarations[NUM];
    };
}