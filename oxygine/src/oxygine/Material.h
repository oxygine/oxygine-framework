#pragma once
#include "oxygine-include.h"
#include "core/Renderer.h"
#include "core/ref_counter.h"

namespace oxygine
{
    class MaterialCache;

#define MATX(cl) \
    cl(){\
        typedef bool (*fn)(const cl&a, const cl&b);\
        fn f = &cl::cmp;\
        _compare = (compare)f;\
        init();\
    }\
    void copyTo(cl &d) const{d = *this;}\
    void copyFrom(const cl &d) {*this = d;}\
    cl* clone() const override {return new cl(*this);}\
    void update(size_t &hash, compare &cm) const override {\
        typedef bool (*fn)(const cl&a, const cl&b);\
        fn f = &cl::cmp;\
        cm = (compare)f;\
        hash = 0;\
        rehash(hash);\
    }



    class Material : public ref_counter
    {
    public:

        static spMaterialX current;
        static spMaterialX null;

        typedef bool(*compare)(const Material* a, const Material* b);

        Material();

        Material& operator = (const Material& r);
        Material(compare cmp);
        Material(const Material& other);


        size_t _hash;
        compare _compare;

        virtual void init() {}

        virtual void xapply() {}
        virtual void xflush() {}

        virtual Material* clone() const = 0;
        virtual void update(size_t& hash, compare&) const = 0;
        virtual void rehash(size_t& hash) const = 0;

        virtual void render(const AffineTransform& tr, const Color& c, const RectF& src, const RectF& dest) {}
        virtual void render(const Color& c, const RectF& src, const RectF& dest) {}

        void apply();
        void flush();


        template <class T>
        void apply2(const T& f)
        {
            apply();
            f();
        }
    };

    typedef intrusive_ptr<Material> spMaterialX;


    class NullMaterialX : public Material
    {
    public:
        MATX(NullMaterialX);
        static bool cmp(const NullMaterialX& a, const NullMaterialX& b) { return false; }
        void rehash(size_t& hash) const override {}
    };

    DECLARE_SMART(STDMaterial, spSTDMaterial);
    class STDMaterial: public Material
    {
    public:
        MATX(STDMaterial);

        spNativeTexture    _base;
        spNativeTexture    _alpha;
        blend_mode         _blend;
        UberShaderProgram* _uberShader;
        Color              _addColor;
        int                _flags;

        static bool cmp(const STDMaterial& a, const STDMaterial& b);

        void init() override;
        void rehash(size_t& hash) const override;

        void xapply() override;
        void xflush() override;

        void render(const AffineTransform& tr, const Color& c, const RectF& src, const RectF& dest) override;
        void render(const Color& c, const RectF& src, const RectF& dest) override;

        spSTDMaterial cloneDefaultShader() const;
    };

}