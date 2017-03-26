#pragma once
#include "core/ref_counter.h"
#include "core/Renderer.h"

namespace oxygine
{


#define MATX(cl) \
    cl(){\
        typedef bool (*fn)(const cl&a, const cl&b);\
        fn f = &cl::cmp;\
        _compare = (compare)f;\
        init();\
    }\
    void copyFrom(const MaterialX &r) override {*this = (cl&)r;}\
    cl* clone() const override {return new cl(*this);}\
    void update(size_t &hash, compare &cm) const override {\
        typedef bool (*fn)(const cl&a, const cl&b);\
        fn f = &cl::cmp;\
        cm = (compare)f;\
        hash = 0;\
        rehash(hash);\
    }



    class MaterialX : public ref_counter
    {
    public:

        static spMaterialX current;

        typedef bool(*compare)(const MaterialX* a, const MaterialX* b);

        MaterialX();

        MaterialX& operator = (const MaterialX& r);
        MaterialX(compare cmp);
        MaterialX(const MaterialX& other);


        size_t _hash;
        compare _compare;

        virtual void init() {}
        
        virtual void xapply() {}
        virtual void xflush() {}

        virtual MaterialX* clone() const = 0;
        virtual void copyFrom(const MaterialX& r) = 0;
        virtual void update(size_t& hash, compare&) const = 0;
        virtual void rehash(size_t& hash) const = 0;

        virtual void render(const AffineTransform &tr, const Color& c, const RectF& src, const RectF& dest);
        virtual void render(const Color& c, const RectF& src, const RectF& dest);

        void apply();
        void flush();

    };

    typedef intrusive_ptr<MaterialX> spMaterialX;

    template<class T>
    class MaterialTX : public MaterialX
    {
    public:
        MATX(MaterialTX<T>);

        typedef bool(*fcmp)(const MaterialTX<T>& a, const MaterialTX<T>& b);

        T data;


        MaterialTX(const T& dat) : data(dat)
        {
        }

        void rehash(size_t& hash) const override
        {
            data.init(hash);
        }

        template<class C>
        static bool cmp(const MaterialTX<C>& a, const MaterialTX<C>& b)
        {
            return a.data.cmp(b.data);
        }

        void xapply() override
        {
            data.apply();
        }

        void xflush() override
        {
            data.flush();
        }

    };

    /*
    class STDMaterialX: public MaterialX
    {
    public:
        MATX(STDMaterialX);

        spNativeTexture    _base;
        spNativeTexture    _alpha;
        blend_mode         _blend;
        UberShaderProgram* _uberShader;
        int                _flags;

    };
    */



    inline void hash_combine(std::size_t& seed) { }

    template <typename T, typename... Rest>
    inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        hash_combine(seed, rest...);
    }

    class STDMatData
    {
    public:
        STDMatData();

        spNativeTexture    _base;
        spNativeTexture    _alpha;
        blend_mode         _blend;
        UberShaderProgram* _uberShader;
        int                _flags;

        void init(size_t& hash) const;
        void apply();
        void flush();
        bool cmp(const STDMatData& b) const;
    };


    typedef MaterialTX<STDMatData> STDMaterialX;

    typedef intrusive_ptr< MaterialTX<STDMatData> > spSTDMaterialX;

    class MaterialCache
    {
    public:

        typedef std::vector<spMaterialX> materials;
        materials _materials;


        template<class T>
        intrusive_ptr<T> cache(const T& other)
        {
            size_t hash;
            MaterialX::compare cm;
            other.update(hash, cm);

            T* fre = 0;
            for (auto m_ : _materials)
            {
                MaterialX* mat = m_.get();
                if (mat->_compare != cm)
                    continue;
                if (mat->_ref_counter == 2)
                    fre = (T*)mat;
                if (mat->_hash != hash)
                    continue;
                bool same = cm(mat, &other);
                if (same)
                    return (T*)mat;
            }
            if (fre)
            {
                fre->copyFrom(other);
                fre->_hash = hash;
                fre->_compare = cm;
                return fre;
            }

            T* copy = other.clone();
            copy->_hash = hash;
            copy->_compare = cm;
            _materials.push_back(copy);

            return copy;
        }
    };

    inline MaterialCache& mc()
    {
        static MaterialCache cache;
        return cache;
    }


    extern spMaterialX currentMat;

}