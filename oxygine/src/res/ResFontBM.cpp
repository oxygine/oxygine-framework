#include "ResFontBM.h"
#include "Font.h"
#include <vector>
#include "pugixml/pugixml.hpp"
#include "Image.h"
#include "core/NativeTexture.h"
#include "core/VideoDriver.h"
#include "core/file.h"
#include "CreateResourceContext.h"
#include "utils/stringUtils.h"
#include "core/Mem2Native.h"
#include "Resources.h"
namespace oxygine
{
    Resource* ResFontBM::create(CreateResourceContext& context)
    {
        ResFontBM* font = 0;

        font = new ResFontBM();
        font->_createFont(&context, false, false, 1);
        setNode(font, context.walker.getNode());
        context.resources->add(font);

        //context.meta = context.meta.next_sibling();

        return font;
    }

    Resource* ResFontBM::createBM(CreateResourceContext& context)
    {
        ResFontBM* font = 0;

        font = new ResFontBM();
        font->_createFont(&context, false, true, 1);
        setNode(font, context.walker.getNode());
        context.resources->add(font);

        //context.meta = context.meta.next_sibling();

        return font;
    }

    Resource* ResFontBM::createSD(CreateResourceContext& context)
    {
        ResFontBM* font = 0;

        font = new ResFontBM();
        font->_createFont(&context, true, false, 1);
        setNode(font, context.walker.getNode());
        context.resources->add(font);

        //context.meta = context.meta.next_sibling();

        return font;
    }


    ResFontBM::ResFontBM(): _font(0), _format(TF_R8G8B8A8), _premultipliedAlpha(false), _sdf(false)
    {

    }

    ResFontBM::~ResFontBM()
    {
        cleanup();
    }


    bool ResFontBM::isSDF(int& size) const
    {
        size = _font->getSize();
        return _sdf;
    }

    const oxygine::Font* ResFontBM::getClosestFont(float worldScale, int styleFontSize, float& resScale) const
    {
        if (styleFontSize)
            resScale = _size / float(styleFontSize);
        else
            resScale = _font->getScale();

        return _font;
    }

    void ResFontBM::init(const char* path, bool premultipliedAlpha)
    {
        _premultipliedAlpha = premultipliedAlpha;
        _file = path;
        _createFont(0, false, false, 1);
    }

    void ResFontBM::initSD(const char* fntPath, int downsample)
    {
        _sdf = true;
        _premultipliedAlpha = true;
        _file = fntPath;
        _createFont(0, true, false, downsample);
    }

    void ResFontBM::cleanup()
    {
        for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
        {
            const page& p = *i;
            p.texture->release();
        }
        _pages.clear();
        delete _font;
        _font = 0;
        _loadCounter = 0;
    }

    void ResFontBM::_restore(Restorable* r, void*)
    {
        void* object = r->_getRestorableObject();
        for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
        {
            const page& p = *i;
            if (p.texture.get() == object)
            {
                _loadPage(p, &RestoreResourcesContext::instance);
            }
        }
    }

    void ResFontBM::_loadPage(const page& p, LoadResourcesContext* load_context)
    {
        if (!load_context->isNeedProceed(p.texture))
            return;

        spImage mt = new Image;

        file::buffer bf;
        file::read(p.file, bf);

        mt->init(bf, !_premultipliedAlpha, _format);
        CreateTextureTask opt;
        opt.src = mt;
        opt.dest = p.texture;
        load_context->createTexture(opt);
        p.texture->reg(CLOSURE(this, &ResFontBM::_restore), 0);
    }

    void ResFontBM::_load(LoadResourcesContext* load_context)
    {
        OX_ASSERT(!_pages.empty());
        if (_pages.empty())
            return;

        for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
        {
            const page& p = *i;
            _loadPage(p, load_context);

        }
    }

    int ucs2_to_utf8(int ucs2, unsigned char* utf8)
    {
        if (ucs2 < 0x80)
        {
            utf8[0] = ucs2;
            utf8[1] = '\0';
            return 1;
        }
        if (ucs2 >= 0x80  && ucs2 < 0x800)
        {
            utf8[0] = (ucs2 >> 6)   | 0xC0;
            utf8[1] = (ucs2 & 0x3F) | 0x80;
            utf8[2] = '\0';
            return 2;
        }
        if (ucs2 >= 0x800 && ucs2 < 0xFFFF)
        {
            utf8[0] = ((ucs2 >> 12)) | 0xE0;
            utf8[1] = ((ucs2 >> 6) & 0x3F) | 0x80;
            utf8[2] = ((ucs2) & 0x3F) | 0x80;
            utf8[3] = '\0';
            return 3;
        }
        return -1;
    }


    ////////////////////////////////////////////////////
    char* _parse(char* data, char** key, char** value, bool& endOfLine, bool& prmFound)
    {
        prmFound = false;
        *key = NULL;
        *value = NULL;

        while ((*data == ' ') || (*data == '\r'))
            data++;
        if (*data == '\n')
        {
            endOfLine = true;
            *data = 0;
            data++;
            return data;
        }

        *key = data;
        while (*data != '\0') // we get key
        {
            if ((*data == ' ') || (*data == '\n'))
            {
                if (*data == '\n')
                    endOfLine = true;
                *data = 0;
                data++;
                return data;
            }
            data++;

            if (*data == '=')
            {
                *data = 0;
                break;
            }
        }
        data++;

        while ((*data == '\"') || (*data == '\r'))
            data++;
        *value = data;
        prmFound = true;

        while (*data != '\0') // we get value
        {
            if ((*data == '\"') || (*data == '\r'))
            {
                *data = 0;
                data++;
            }
            if ((*data == ' ') || (*data == '\n'))
            {
                if (*data == '\n')
                    endOfLine = true;
                *data = 0;
                data++;
                return data;
            }
            data++;
        }

        *data = 0;
        return data;
    }

    void ResFontBM::_createFontFromTxt(CreateResourceContext* context, char* fontData, const std::string& fontPath, int downsample)
    {
        char* key = 0;
        char* value = 0;
        bool endOfLine = false;
        bool prmFound = false;

        // info block
        int fontSize = 0;
        while (*(fontData = _parse(fontData, &key, &value, endOfLine, prmFound)) != 0)
        {
            if (prmFound)
            {
                if (!strcmp(key, "size"))
                    fontSize = atoi(value);
            }
            if (endOfLine)
            {
                endOfLine = false;
                break;
            }
        }

        // common block
        int nPages = 0;
        int lineHeight = 0;
        int base = 0;
        int tw = 0;
        int th = 0;
        while (*(fontData = _parse(fontData, &key, &value, endOfLine, prmFound)) != 0)
        {
            if (prmFound)
            {
                if (!strcmp(key, "lineHeight"))
                    lineHeight = atoi(value);
                else if (!strcmp(key, "base"))
                    base = atoi(value);
                else if (!strcmp(key, "scaleW"))
                    tw = atoi(value);
                else if (!strcmp(key, "scaleH"))
                    th = atoi(value);
                else if (!strcmp(key, "pages"))
                    nPages = atoi(value);
            }
            if (endOfLine)
            {
                endOfLine = false;
                break;
            }
        }

        tw /= downsample;
        th /= downsample;
        lineHeight /= downsample;
        base /= downsample;
        fontSize /= downsample;

        char tail[255];
        char head[255];
        path::split(fontPath.c_str(), head, tail);

        // page blocks
        for (int i = 0; i < nPages; i++)
        {
            while (*(fontData = _parse(fontData, &key, &value, endOfLine, prmFound)) != 0)
            {
                if (prmFound)
                {
                    if (!strcmp(key, "file"))
                        addPage(tw, th, head, value);
                }
                if (endOfLine)
                {
                    endOfLine = false;
                    break;
                }
            }
        }

        // create font
        if (!tw)
            load(0);

        fontSize = abs(fontSize);
        Font* font = new Font();
        font->init(getName().c_str(), fontSize, fontSize, lineHeight + fontSize - base, _sdf);
        _size = fontSize;
        _font = font;

        if (context)
        {
            float scale = context->walker.getMeta().attribute("sf").as_float(1.0f) / context->walker.getScaleFactor();
            _font->setScale(scale);
        }

        // chars block
        int numChars = 0;
        while (*(fontData = _parse(fontData, &key, &value, endOfLine, prmFound)) != 0)
        {
            if (prmFound)
            {
                if (!strcmp(key, "count"))
                    numChars = atoi(value);
            }
            if (endOfLine)
            {
                endOfLine = false;
                break;
            }
        }

        // chars blocks
        for (int i = 0; i < numChars; i++)
        {
            int charID = -1;
            int xpos = 0;
            int ypos = 0;
            int width = 0;
            int height = 0;
            int xoffset = 0;
            int yoffset = 0;
            int xadvance = 0;
            int page_ = 0;

            while (*(fontData = _parse(fontData, &key, &value, endOfLine, prmFound)) != 0)
            {
                if (prmFound)
                {
                    if (!strcmp(key, "id"))
                        charID = atoi(value);
                    else if (!strcmp(key, "x"))
                        xpos = atoi(value);
                    else if (!strcmp(key, "y"))
                        ypos = atoi(value);
                    else if (!strcmp(key, "width"))
                        width = atoi(value);
                    else if (!strcmp(key, "height"))
                        height = atoi(value);
                    else if (!strcmp(key, "xoffset"))
                        xoffset = atoi(value);
                    else if (!strcmp(key, "yoffset"))
                        yoffset = atoi(value);
                    else if (!strcmp(key, "xadvance"))
                        xadvance = atoi(value);
                    else if (!strcmp(key, "page"))
                        page_ = atoi(value);
                }
                if (endOfLine)
                {
                    endOfLine = false;
                    break;
                }
            }

            spTexture t = _pages[page_].texture;
            float iw = 1.0f / t->getWidth() / downsample;
            float ih = 1.0f / t->getHeight() / downsample;

            glyph gl;
            gl.src = RectF(xpos * iw, ypos * ih, width * iw, height * ih);
            gl.sw = width / downsample;
            gl.sh = height / downsample;
            gl.offset_x = xoffset / downsample;
            gl.offset_y = yoffset / downsample - base;
            gl.advance_x = xadvance / downsample;
            gl.advance_y = 0;

            int code = 0;
            ucs2_to_utf8(charID, (unsigned char*)&code);
            gl.ch = code;
            gl.opt = 0;
            gl.texture = _pages[page_].texture;

            _font->addGlyph(gl);
        }

        _font->sortGlyphs();
        _finalize();
    }
    /////////////////////////////////////////////////////////

    void ResFontBM::addPage(int tw, int th, const char* head, const char* textureFile)
    {
        page p;
        if (*head)
        {
            p.file = head;
            p.file += "//";
        }
        p.file += textureFile;
        p.texture = IVideoDriver::instance->createTexture();
        p.texture->setName(p.file);

        if (tw)
            p.texture->init(0, tw, th, TF_UNDEFINED);

        _pages.push_back(p);
    }

    void ResFontBM::_finalize()
    {
        glyphOptions opt = 0;
        const glyph* g = _font->getGlyph(0xA0, opt);
        if (g)
            return;

        g = _font->getGlyph(' ', opt);
        if (!g)
            return;

        glyph p = *g;
        p.ch = 0xA0;
        _font->addGlyph(p);

    }

    void ResFontBM::_createFont(CreateResourceContext* context, bool sd, bool bmc, int downsample)
    {
        _sdf = sd;
        if (sd)
            _format = TF_L8;

        if (context)
        {
            pugi::xml_node node = context->walker.getNode();

            downsample = node.attribute("downsample").as_int(1);
            _premultipliedAlpha = node.attribute("premultiplied_alpha").as_bool(_premultipliedAlpha);

            _file = context->walker.getPath("file");
            setName(Resource::extractID(node, _file, ""));

            if (bmc)
            {
                _file = *context->prebuilt_folder + getName() + ".fnt";
            }
        }

        std::string path = _file;
        file::buffer fb;
        file::read(path, fb);

        if (fb.empty())
            return;

        if (!(fb[0] == '<' && fb[1] == '?' && fb[2] == 'x'))
        {
            _createFontFromTxt(context, reinterpret_cast<char*>(fb.getData()), path, downsample);
            return;
        }
        /////////////////////////////////////////////////

        pugi::xml_document doc;
        doc.load_buffer_inplace(&fb.data[0], fb.data.size());


        pugi::xml_node root = doc.first_child();
        pugi::xml_node info = root.child("info");

        //<info face="Century Gothic" size="-24" bold="0" italic="0" charset="" unicode="1" stretchH="100" smooth="1" aa="1" padding="0,0,0,0" spacing="1,1" outline="0"/>
        int fontSize = info.attribute("size").as_int() / downsample;




        pugi::xml_node common = info.next_sibling("common");
        int lineHeight = common.attribute("lineHeight").as_int() / downsample;
        int base = common.attribute("base").as_int() / downsample;

        pugi::xml_node pages = common.next_sibling("pages");

        int tw = common.attribute("scaleW").as_int();
        int th = common.attribute("scaleH").as_int();


        tw /= downsample;
        th /= downsample;


        char folder[255];
        char tail[255];
        path::split(path.c_str(), folder, tail);

        for (pugi::xml_node page_node = pages.child("page"); page_node; page_node = page_node.next_sibling("page"))
        {
            const char* textureFile = page_node.attribute("file").value();
            addPage(tw, th, folder, textureFile);
        }


        if (!tw)
            load(0);


        fontSize = abs(fontSize);
        Font* font = new Font();
        font->init(getName().c_str(), fontSize, fontSize, lineHeight + fontSize - base, _sdf);
        _size = fontSize;
        _font = font;

        if (context)
        {
            float scale = context->walker.getMeta().attribute("sf").as_float(1.0f) / context->walker.getScaleFactor();
            _font->setScale(scale);
        }

        pugi::xml_node chars = pages.next_sibling("chars");
        pugi::xml_node child = chars.first_child();
        while (!child.empty())
        {
            int charID = 0;
            int xpos = 0;
            int ypos = 0;
            int width = 0;
            int height = 0;
            int xoffset = 0;
            int yoffset = 0;
            int xadvance = 0;
            int page = 0;

            pugi::xml_attribute attr = child.first_attribute();
            while (!attr.empty())
            {
                const char* attr_name = attr.name();
                int value = attr.as_int();

                if (!strcmp(attr_name, "id"))
                    charID = value;
                else if (!strcmp(attr_name, "x"))
                    xpos = value;
                else if (!strcmp(attr_name, "y"))
                    ypos = value;
                else if (!strcmp(attr_name, "width"))
                    width = value;
                else if (!strcmp(attr_name, "height"))
                    height = value;
                else if (!strcmp(attr_name, "xoffset"))
                    xoffset = value;
                else if (!strcmp(attr_name, "yoffset"))
                    yoffset = value;
                else if (!strcmp(attr_name, "xadvance"))
                    xadvance = value;
                else if (!strcmp(attr_name, "page"))
                    page = value;

                attr = attr.next_attribute();
            }

            spTexture t = _pages[page].texture;
            float iw = 1.0f / t->getWidth() / downsample;
            float ih = 1.0f / t->getHeight() / downsample;

            glyph gl;
            gl.src = RectF(xpos * iw, ypos * ih, width * iw, height * ih);
            gl.sw = width / downsample;
            gl.sh = height / downsample;
            gl.offset_x = xoffset / downsample;
            gl.offset_y = yoffset / downsample - base;
            gl.advance_x = xadvance / downsample;
            gl.advance_y = 0;

            int code = 0;
            ucs2_to_utf8(charID, (unsigned char*)&code);
            gl.ch = code;
            gl.opt = 0;
            gl.texture = _pages[page].texture;

            font->addGlyph(gl);

            child = child.next_sibling();
        }

        font->sortGlyphs();
        _finalize();
    }

    void ResFontBM::_unload()
    {
        OX_ASSERT(!_pages.empty());
        for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
        {
            const page& p = *i;
            p.texture->release();
        }
    }

    const Font* ResFontBM::getFont(const char* name, int size) const
    {
        return _font;
    }
}
