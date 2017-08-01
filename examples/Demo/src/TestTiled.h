#pragma once
#include "test.h"
#include "core/file.h"
#include "res/SingleResAnim.h"

// Bits on the far end of the 32-bit global tile ID are used for tile flags
const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

DECLARE_SMART(Tiled, spTiled);
class Tiled: public Sprite
{
public:

    struct layer
    {
        vector<unsigned int> tiles;
    };

    list< layer > layers;


    int width;
    int height;

    spNativeTexture nt;

    int tileWidth;
    int tileHeight;

    int cols;
    int rows;

    void createTileSetTexture(Image& src)
    {
        cols = src.getWidth() / tileWidth;
        rows = src.getHeight() / tileHeight;

        Image dest;
        dest.init(cols * (tileWidth + 2), rows * (tileHeight * 2), TF_R8G8B8A8);


        //http://stackoverflow.com/questions/19611745/opengl-black-lines-in-between-tiles
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < cols; ++x)
            {
                Rect srcRect(x * tileWidth, y * tileHeight, tileWidth, tileHeight);
                Rect destRect = srcRect;
                destRect.pos.x += 2 * x + 1;
                destRect.pos.y += 2 * y + 1;

                ImageData srcIm = src.lock(srcRect);
                ImageData destIm = dest.lock(destRect);
                operations::blit(srcIm, destIm);

                destRect.expand(Point(1, 1), Point(1, 1));



                operations::blit(
                    dest.lock(destRect.pos.x + 1, destRect.pos.y + 1, tileWidth, 1),
                    dest.lock(destRect.pos.x + 1, destRect.pos.y, tileWidth, 1));

                operations::blit(
                    dest.lock(destRect.pos.x + 1, destRect.pos.y + tileHeight, tileWidth, 1),
                    dest.lock(destRect.pos.x + 1, destRect.pos.y + tileHeight + 1, tileWidth, 1));

                operations::blit(
                    dest.lock(destRect.pos.x + 1, destRect.pos.y, 1, tileHeight + 2),
                    dest.lock(destRect.pos.x, destRect.pos.y, 1, tileHeight + 2));

                operations::blit(
                    dest.lock(destRect.pos.x + tileWidth, destRect.pos.y, 1, tileHeight + 2),
                    dest.lock(destRect.pos.x + tileWidth + 1, destRect.pos.y, 1, tileHeight + 2));

            }
        }

        nt = IVideoDriver::instance->createTexture();
        nt->init(dest.lock());
        nt->setClamp2Edge(true);
        nt->setLinearFilter(false);
    }

    Tiled(const std::string& tmx, const std::string& texture)
    {
        file::buffer fb;
        //read tmx into buffer
        file::read(tmx, fb);

        pugi::xml_document doc;
        //parse xml
        doc.load_buffer(fb.getData(), fb.getSize());

        pugi::xml_node map = doc.document_element();
        width = map.attribute("width").as_int();
        height = map.attribute("height").as_int();


        tileWidth = map.attribute("tilewidth").as_int();
        tileHeight = map.attribute("tileheight").as_int();



        //load layers
        pugi::xml_node layerNode = map.child("layer");
        while (!strcmp(layerNode.name(), "layer"))
        {
            pugi::xml_node data = layerNode.child("data");
            pugi::xml_node tile = data.first_child();

            layer l;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    l.tiles.push_back(tile.attribute("gid").as_uint());
                    tile = tile.next_sibling();
                }
            }

            layers.push_back(l);

            layerNode = layerNode.next_sibling();
        }

        //load image from file
        Image src;
        file::read(texture, fb);
        src.init(fb, true);

        createTileSetTexture(src);


        //saveImage(dest.lock(), "test.tga");
    }

    void drawLayer(const Transform& transform, int startX, int startY, int endX, int endY, const layer& l)
    {
        const std::vector<unsigned int>& tiles = l.tiles;

        Color color(Color::White);

        STDRenderer* renderer = STDRenderer::instance;

        float tw = 1.0f / nt->getWidth();
        float th = 1.0f / nt->getHeight();


        for (int y = startY; y < endY; ++y)
        {
            for (int x = startX; x < endX; ++x)
            {
                unsigned int tile = tiles[y * width + x];
                if (!tile)
                    continue;

                tile = tile - 1;

                bool flipped_horizontally = (tile & FLIPPED_HORIZONTALLY_FLAG) ? true : false;
                bool flipped_vertically = (tile & FLIPPED_VERTICALLY_FLAG) ? true : false;
                bool flipped_diagonally = (tile & FLIPPED_DIAGONALLY_FLAG) ? true : false;

                // Clear the flags
                tile &= ~(FLIPPED_HORIZONTALLY_FLAG |
                          FLIPPED_VERTICALLY_FLAG |
                          FLIPPED_DIAGONALLY_FLAG);


                int col = tile % cols;
                int row = tile / cols;


                Rect src(col * (tileWidth + 2) + 1, row * (tileHeight + 2) + 1, tileWidth, tileHeight);

                RectF srcUV = src.cast<RectF>();
                srcUV.pos.x *= tw;
                srcUV.pos.y *= th;
                srcUV.size.x *= tw;
                srcUV.size.y *= th;

                Rect dest(x * tileWidth, y * tileHeight, tileWidth, tileHeight);
                RectF destF = dest.cast<RectF>();


                //based on fillQuadT
                unsigned int rgba = color.premultiplied().rgba();
                const RectF& srcRect = srcUV;
                const RectF& destRect = destF;

                float u = srcRect.pos.x;
                float v = srcRect.pos.y;

                float du = srcRect.size.x;
                float dv = srcRect.size.y;

                vertexPCT2 vert[4];
                vertexPCT2 vt;
                vertexPCT2* pv = vert;
                vt.color = rgba;

                const Vector2& pos = destRect.pos;
                const Vector2& size = destRect.size;

                Vector2 p1(pos.x, pos.y);
                Vector2 p2(pos.x, pos.y + size.y);
                Vector2 p3(pos.x + size.x, pos.y);
                Vector2 p4(pos.x + size.x, pos.y + size.y);


                p1 = transform.transform(p1);
                p2 = transform.transform(p2);
                p3 = transform.transform(p3);
                p4 = transform.transform(p4);

                if (flipped_horizontally)
                {
                    std::swap(p1, p3);
                    std::swap(p2, p4);
                }

                if (flipped_vertically)
                {
                    std::swap(p1, p2);
                    std::swap(p3, p4);
                }


                if (flipped_diagonally)
                {
                    Vector2 t = p3;
                    p3 = p4;
                    p4 = p2;
                    p2 = p1;
                    p1 = t;

                    std::swap(p1, p2);
                    std::swap(p3, p4);
                }

                vt.z = 0;

                vt.x = p1.x;
                vt.y = p1.y;
                vt.u = u;
                vt.v = v;
                *pv = vt;
                ++pv;

                vt.x = p2.x;
                vt.y = p2.y;
                vt.u = u;
                vt.v = v + dv;
                *pv = vt;
                ++pv;

                vt.x = p3.x;
                vt.y = p3.y;
                vt.u = u + du;
                vt.v = v;
                *pv = vt;
                ++pv;

                vt.x = p4.x;
                vt.y = p4.y;
                vt.u = u + du;
                vt.v = v + dv;
                *pv = vt;
                ++pv;

                renderer->addVertices(vert, sizeof(vert));
            }
        }
    }

    void doRender(const RenderState& rs)
    {
        Material::setCurrent(rs.material);

        STDRenderer* renderer = STDRenderer::instance;
        renderer->setTexture(nt);
        renderer->setTransform(rs.transform);
        renderer->setBlendMode(blend_premultiplied_alpha);


        Transform world = rs.transform;
        world.invert();

        //find top left local position of TiledActor visible on display
        Vector2 topLeft = world.transform(Vector2(0, 0));

        //find bottom right local position of TiledActor visible on display
        Vector2 bottomRight = world.transform(getStage()->getSize());

        //we don't want to draw tiles outside of visible area
        int startX = std::max(0,      int(topLeft.x / tileWidth));
        int startY = std::max(0,      int(topLeft.y / tileHeight));
        int endX   = std::min(width,  int(bottomRight.x / tileWidth) + 1);
        int endY   = std::min(height, int(bottomRight.y / tileHeight) + 1);

        for (std::list<layer>::const_iterator i = layers.begin(); i != layers.end(); ++i)
            drawLayer(rs.transform, startX, startY, endX, endY, *i);
    }
};



class TestTiled : public Test
{
public:


    TestTiled()
    {
        spTiled tiled = new Tiled("etc/orthogonal-outside.tmx", "etc/buch-outdoor.png");

        //use camera class from other test!
        spCamera cam = new Camera;
        cam->attachTo(content);
        cam->setContent(tiled);
        cam->setSize(content->getSize());
    }
};
