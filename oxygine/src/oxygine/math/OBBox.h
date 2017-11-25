#pragma once
#include "../oxygine-include.h"
#include "Rect.h"
#include "Vector2.h"

namespace oxygine
{
    class OBBox
    {
    private:
        /** Corners of the box, where 0 is the lower left. */
        Vector2         corner[4];

        /** Two edges of the box extended away from corner[0]. */
        Vector2         axis[2];

        /** origin[a] = corner[0].dot(axis[a]); */
        float          origin[2];

        /** Returns true if other overlaps one dimension of this. */
        bool overlaps1Way(const OBBox& other) const
        {
            for (int a = 0; a < 2; ++a)
            {

                float t = other.corner[0].dot(axis[a]);

                // Find the extent of box 2 on axis a
                float tMin = t;
                float tMax = t;

                for (int c = 1; c < 4; ++c)
                {
                    t = other.corner[c].dot(axis[a]);

                    if (t < tMin)
                    {
                        tMin = t;
                    }
                    else if (t > tMax)
                    {
                        tMax = t;
                    }
                }

                // We have to subtract off the origin

                // See if [tMin, tMax] intersects [0, 1]
                if ((tMin > 1 + origin[a]) || (tMax < origin[a]))
                {
                    // There was no intersection along this dimension;
                    // the boxes cannot possibly overlap.
                    return false;
                }
            }

            // There was no dimension along which there is no intersection.
            // Therefore the boxes overlap.
            return true;
        }


        /** Updates the axes after the corners move.  Assumes the
        corners actually form a rectangle. */
        void computeAxes()
        {
            axis[0] = corner[1] - corner[0];
            axis[1] = corner[3] - corner[0];

            // Make the length of each axis 1/edge length so we know any
            // dot product must be less than 1 to fall within the edge.

            for (int a = 0; a < 2; ++a)
            {
                axis[a] /= axis[a].sqlength();
                origin[a] = corner[0].dot(axis[a]);
            }
        }

    public:

        OBBox(const RectF& rect, const AffineTransform& tr)
        {
            corner[0] = tr.transform(rect.getLeftTop());
            corner[1] = tr.transform(rect.getRightTop());
            corner[2] = tr.transform(rect.getRightBottom());
            corner[3] = tr.transform(rect.getLeftBottom());

            computeAxes();
        }

        /** Returns true if the intersection of the boxes is non-empty. */
        bool overlaps(const OBBox& other) const
        {
            return overlaps1Way(other) && other.overlaps1Way(*this);
        }

        RectF aabbox() const
        {
            RectF rect(corner[0], Vector2(0, 0));
            rect.unite(corner[1]);
            rect.unite(corner[2]);
            rect.unite(corner[3]);
            return rect;
        }

    };

}