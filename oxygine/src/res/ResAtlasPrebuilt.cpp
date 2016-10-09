#include "ResAtlasPrebuilt.h"
#include "Resources.h"

extern "C"
{
#include "utils/cdecode.h"
}

namespace oxygine
{

    ResAtlasPrebuilt::ResAtlasPrebuilt(CreateResourceContext& context)
    {
        pugi::xml_node node = context.walker.getNode();
        pugi::xml_node meta = context.walker.getMeta();

        const char* format = node.attribute("format").as_string("8888");

        loadBase(node);


        TextureFormat tf = string2TextureFormat(format);

        pugi::xml_node meta_image = meta.child("atlas");

        pugi::xml_node meta_alpha = meta.child("ht");
        if (meta_alpha)
        {
            const char* data = meta_alpha.text().as_string();
            int len = meta_alpha.attribute("len").as_int();
            base64_decodestate state;
            base64_init_decodestate(&state);
            _hitTestBuffer.resize(len * 3 / 4);
            if (len)
                base64_decode_block(data, len, (char*)&_hitTestBuffer.front(), &state);
        }

        bool compressed = false;

        while (meta_image)
        {
            const char* file = meta_image.attribute("file").value();

            int w = meta_image.attribute("w").as_int();
            int h = meta_image.attribute("h").as_int();

            const char* file_format = meta_image.attribute("format").as_string(0);
            TextureFormat ffmt = TF_UNDEFINED;
            if (file_format)
            {
                ffmt = string2TextureFormat(file_format);
                compressed = isCompressedFormat(ffmt);
            }

            std::string alpha_file = meta_image.attribute("alpha").as_string("");
            if (!alpha_file.empty())
            {
                alpha_file = *context.prebuilt_folder + alpha_file;
            }

            std::string p = *context.prebuilt_folder + file;
            addAtlas(tf, p, alpha_file, w, h);

            meta_image = meta_image.next_sibling("atlas");
            context.walker.nextMeta();
        }

        while (true)
        {
            XmlWalker walker = context.walker.next();
            if (walker.empty())
                break;

            pugi::xml_node child_node = walker.getNode();

            const char* name = child_node.name();
            if (strcmp(name, "image"))
                continue;

            pugi::xml_node meta_frames = walker.getMeta();

            std::string id = child_node.attribute("id").value();
            std::string file = child_node.attribute("file").value();

            if (file.empty())
            {
                createEmpty(walker, context);
                continue;
            }


            OX_ASSERT(meta_frames && "Did you recreate atlasses?");

            int columns = 0;
            int rows = 0;
            int frame_width = 0;
            int frame_height = 0;
            float frame_scale = 1.0f;


            const char* frame_size = meta_frames.attribute("fs").value();

            sscanf(frame_size, "%d,%d,%d,%d,%f", &columns, &rows,
                   &frame_width, &frame_height,
                   &frame_scale);

            HitTestData adata;
            const char* alpha = meta_frames.attribute("ht").as_string(0);
            if (alpha)
            {
                int ad_w, ad_h, ad_pos, ad_size;
                sscanf(alpha, "%d,%d,%d,%d", &ad_pos, &ad_size, &ad_w, &ad_h);
                adata.w = ad_w;
                adata.h = ad_h;
                adata.pitch = ad_size / ad_h;
                adata.data = &_hitTestBuffer[ad_pos];
            }



            if (columns)
            {
                animationFrames frames;
                int frames_count = rows * columns;
                frames.reserve(frames_count);

                ResAnim* ra = new ResAnim(this);


                OX_ASSERT(meta_frames);


                char* frames_data = (char*)meta_frames.first_child().value();


                const char* begin = frames_data;
                while (*frames_data)
                {
                    if (*frames_data == ';')
                    {
                        *frames_data = 0;
                        int id = 0;
                        int x = 0;
                        int y = 0;

                        int bbox_x = 0;
                        int bbox_y = 0;
                        int bbox_w = 0;
                        int bbox_h = 0;

                        sscanf(begin, "%d,%d,%d,%d,%d,%d,%d", &id, &x, &y, &bbox_x, &bbox_y, &bbox_w, &bbox_h);

                        begin = frames_data + 1;

                        spNativeTexture& texture = _atlasses[id].base;
                        spNativeTexture& alpha = _atlasses[id].alpha;

                        float iw = 1.0f / texture->getWidth();
                        float ih = 1.0f / texture->getHeight();

                        RectF srcRect(x * iw, y * ih, bbox_w * iw, bbox_h * ih);

                        float fs = frame_scale;
                        RectF destRect(
                            Vector2((float)bbox_x, (float)bbox_y) * fs,
                            Vector2((float)bbox_w, (float)bbox_h) * fs
                        );

                        AnimationFrame frame;
                        Diffuse df;
                        df.base = texture;
                        df.alpha = alpha;

                        //compressed data could not be premultiplied
                        df.premultiplied = !compressed;

                        size_t n = frames.size();
                        int column = n % columns;
                        int row = (int)(n / columns);

                        frame.init2(ra, column, row, df,
                                    srcRect, destRect,
                                    Vector2((float)frame_width, (float)frame_height));
                        if (adata.pitch)
                        {
                            frame.setHitTestData(adata);
                            adata.data += adata.h * adata.pitch;
                        }

                        frames.push_back(frame);
                        if ((int)frames.size() >= frames_count)
                            break;
                    }

                    ++frames_data;
                }

                init_resAnim(ra, file, child_node);

                ra->init(frames, columns, walker.getScaleFactor(), 1.0f / frame_scale);
                ra->setParent(this);
                context.resources->add(ra, context.options->_shortenIDS);
            }
        }
    }
}