#include "TreeInspectorPreview.h"
#include "TreeInspector.h"
#include "Stage.h"
#include "TextField.h"
#include "res/Resources.h"
#include "RenderState.h"
#include "STDRenderer.h"
#include "STDMaterial.h"

namespace oxygine
{
    TreeInspectorPreview::TreeInspectorPreview(TreeInspector* tree): _tree(tree), _prevParent(0), _drawChildren(false)
    {
        setTouchEnabled(false);
    }

    TreeInspectorPreview::~TreeInspectorPreview()
    {

    }

    Vector2 fitSize(const Vector2& destSize, const Vector2& src)
    {
        Vector2 srcSize = src;
        if (srcSize.x < 1)
            srcSize.x = 1;
        if (srcSize.y < 1)
            srcSize.y = 1;

        float scaleX = srcSize.x / destSize.x;
        float scaleY = srcSize.y / destSize.y;

        scaleX = 1.0f / scaleX;
        scaleY = 1.0f / scaleY;

        float scale = scaleX < scaleY ? scaleX : scaleY;
        return Vector2(srcSize.x * scale, srcSize.y * scale);
    }

    void TreeInspectorPreview::init(spActor item)
    {
        STDRenderer r_(&_videoCache);
        STDMaterial mat(&r_);

        STDRenderer& r = r_;
        RenderState rs;
        rs.transform = item->getTransform();
        rs.material = &mat;
        r.begin(0);
        //r.setTransform(rs.transform);
        item->doRender(rs);
        r.end();
        r.drawBatch();

        Material::setCurrent(0);

        setSize(30, 30);

        RectF itemRect = _videoCache._bounds;
        if (itemRect.isEmpty())
        {
            itemRect = item->getDestRect();
            if (itemRect.isEmpty())
                itemRect.setSize(Vector2(10, 4));
        }

        Vector2 ns = fitSize(Vector2(50.0f, 50.0f), itemRect.size);
        float scale = ns.x / itemRect.size.x;

        _cacheTransform.identity();

        _cacheTransform.scale(Vector2(scale, scale));
        _cacheTransform.translate(-itemRect.pos);



        AnimationFrame fr = _tree->_resSystem->getResAnim("checker")->getFrame(0, 0);
        //Point itemSize(30, 30);// = _getItemRect().size;

        RectF srcRect = fr.getSrcRect();
        const Diffuse& df = fr.getDiffuse();
        srcRect.size.x = ns.x / (float)df.base->getWidth();
        srcRect.size.y = ns.y / (float)df.base->getHeight();
        RectF destRect = fr.getDestRect();
        destRect.size = ns;

        AnimationFrame cfr;
        cfr.init(0, df, srcRect, destRect, ns);

        setAnimFrame(cfr);
    }

    void TreeInspectorPreview::doRender(RenderState const& parentRenderState)
    {
        Sprite::doRender(parentRenderState);

        STDMaterial::instance->getRenderer()->drawBatch();
        _videoCache.render(_cacheTransform * parentRenderState.transform);
        STDMaterial::instance->getRenderer()->drawBatch();
        STDMaterial::instance->getRenderer()->resetSettings();
    }
}