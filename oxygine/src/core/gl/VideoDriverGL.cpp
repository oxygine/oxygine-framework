#include "VideoDriverGL.h"
#include "NativeTextureGLES.h"
#include "oxgl.h"
#include "math/Color.h"
#include "utils/stringUtils.h"

namespace oxygine
{
    VideoDriverGL::VideoDriverGL(): _prevFBO(0), _batches(0), _triangles(0),
        _traceStats(true)
    {

    }

    void    VideoDriverGL::getStats(Stats& s) const
    {
        s.batches = _batches;
        s.triangles = _triangles;
    }

    unsigned int VideoDriverGL::getPT(IVideoDriver::PRIMITIVE_TYPE pt)
    {
        switch (pt)
        {
            case PT_POINTS:
                return GL_POINTS;
            case PT_LINES:
                return GL_LINES;
            case PT_LINE_LOOP:
                return GL_LINE_LOOP;
            case PT_LINE_STRIP:
                return PT_LINE_STRIP;
            case PT_TRIANGLES:
                return GL_TRIANGLES;
            case PT_TRIANGLE_STRIP:
                return GL_TRIANGLE_STRIP;
            case PT_TRIANGLE_FAN:
                return GL_TRIANGLE_FAN;
        }
        OX_ASSERT(!"unknown primitive type");
        return PT_POINTS;
    }

    unsigned int VideoDriverGL::getBT(IVideoDriver::BLEND_TYPE pt)
    {
        switch (pt)
        {
            case BT_ZERO:
                return GL_ZERO;
            case BT_ONE:
                return GL_ONE;
            case BT_SRC_COLOR:
                return GL_SRC_COLOR;
            case BT_ONE_MINUS_SRC_COLOR:
                return GL_ONE_MINUS_SRC_COLOR;
            case BT_SRC_ALPHA:
                return GL_SRC_ALPHA;
            case BT_ONE_MINUS_SRC_ALPHA:
                return GL_ONE_MINUS_SRC_ALPHA;
            case BT_DST_COLOR:
                return GL_DST_COLOR;
            case BT_DST_ALPHA:
                return GL_DST_ALPHA;
            case BT_ONE_MINUS_DST_ALPHA:
                return GL_ONE_MINUS_DST_ALPHA;
            case BT_ONE_MINUS_DST_COLOR:
                return GL_ONE_MINUS_DST_COLOR;
        }
        OX_ASSERT(!"unknown blend");
        return GL_ONE;
    }

    void VideoDriverGL::_debugAddPrimitives(IVideoDriver::PRIMITIVE_TYPE pt, int num)
    {
        if (!_traceStats)
            return;

        switch (pt)
        {
            case PT_TRIANGLE_STRIP:
                _triangles += num - 2;
                break;
            case PT_TRIANGLES:
                _triangles += num / 3;
                break;
        }

        _batches++;
    }

    bool VideoDriverGL::getScissorRect(Rect& r) const
    {
        GLboolean scrTest = glIsEnabled(GL_SCISSOR_TEST);

        GLint box[4];
        glGetIntegerv(GL_SCISSOR_BOX, box);
        r = Rect(box[0], box[1], box[2], box[3]);

        CHECKGL();

        return scrTest ? true : false;
    }

    spNativeTexture VideoDriverGL::getRenderTarget() const
    {
        return _rt;
    }

    const VertexDeclarationGL* VideoDriverGL::getVertexDeclaration(bvertex_format fmt) const
    {
        return _vdeclarations.get(fmt);
    }

    void VideoDriverGL::getViewport(Rect& r) const
    {
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);

        r = Rect(vp[0], vp[1], vp[2], vp[3]);;
        //log::messageln("vp %d %d %d %d", vp[0], vp[1], vp[2], vp[3]);
        CHECKGL();
    }

    void VideoDriverGL::setScissorRect(const Rect* rect)
    {
        if (rect)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(rect->getX(), rect->getY(), rect->getWidth(), rect->getHeight());
        }
        else
            glDisable(GL_SCISSOR_TEST);
        CHECKGL();
    }

    void VideoDriverGL::setRenderTarget(spNativeTexture rt)
    {
        _rt = rt;
        if (!rt)
        {
            oxglBindFramebuffer(GL_FRAMEBUFFER, _prevFBO);
            CHECKGL();
            return;
        }

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_prevFBO);

        NativeTextureGLES* gl = safeCast<NativeTextureGLES*>(rt.get());
        oxglBindFramebuffer(GL_FRAMEBUFFER, gl->getFboID());
        CHECKGL();
    }

    void VideoDriverGL::_begin(const Rect& viewport, const Color* clearColor)
    {
        //  log::messageln("begin %d %d %d %d", viewport.pos.x, viewport.pos.y, viewport.size.x, viewport.size.y);
        glViewport(viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight());
        glDisable(GL_SCISSOR_TEST);
        if (clearColor)
        {
            glClearColor(clearColor->getRedF(), clearColor->getGreenF(), clearColor->getBlueF(), clearColor->getAlphaF());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        else
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        CHECKGL();
    }

    void VideoDriverGL::setBlendFunc(BLEND_TYPE src, BLEND_TYPE dest)
    {
        glBlendFunc(getBT(src), getBT(dest));
        CHECKGL();
    }

    void VideoDriverGL::setState(STATE state, unsigned int value)
    {
        switch (state)
        {
            case STATE_BLEND:
                if (value)
                    glEnable(GL_BLEND);
                else
                    glDisable(GL_BLEND);
                break;
            default:
                OX_ASSERT(!"unknown state");
        }
        CHECKGL();
    }

    /*
    void VertexDeclarationGL::init(bvertex_format fmt)
    {
        bformat = fmt;
        int i = 0;
        int offset = 0;
        VertextElement *dest = elements;
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
        for (int j = 0; j < numTexCoords; ++j )
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

    VertexDeclarationGL declarations[8];

    const VertexDeclarationGL *VertexDeclarationGL::getDeclaration(bvertex_format bformat)
    {
        for (int i = 0; i < 8; ++i)
        {
            VertexDeclarationGL &decl = declarations[i];
            if (decl.bformat == 0)
            {
                decl.init(bformat);
            }
            if (decl.bformat == bformat)
                return &decl;
        }

        OX_ASSERT(0);
        return 0;
    }
    */
}