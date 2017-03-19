#pragma once
#include "core/ref_counter.h"
#include "core/Renderer.h"

namespace oxygine
{

    class MaterialX : public ref_counter
    {
    public:

        typedef bool(*compare)(const MaterialX* a, const MaterialX* b);

        MaterialX();
        MaterialX(compare cmp);
        MaterialX(const MaterialX& other);

        size_t _hash;
        compare _compare;

        virtual void apply() = 0;

        virtual MaterialX* clone() const = 0;
    };

    typedef intrusive_ptr<MaterialX> spMaterialX;


    class STDMaterialX : public MaterialX
    {
    public:

    };


    template<class T>
    class MaterialTX : public MaterialX
    {
    public:

        typedef bool(*fcmp)(const MaterialTX<T>& a, const MaterialTX<T>& b);

        T _data;

        MaterialTX(const T& data) : _data(data)
        {
            _data.init(_hash);

            fcmp fn = &MaterialTX<T>::cmp;
            _compare = (compare)(fn);

            hash_combine(_hash, _compare);
        }

        template<class C>
        static bool cmp(const MaterialTX<C>& a, const MaterialTX<C>& b)
        {
            return a._data.isSame(b._data);
        }

        void apply() override
        {
            _data.apply();
        }

        MaterialTX<T>* clone() const override
        {
            MaterialTX<T>* copy = new MaterialTX<T>(*this);
            return copy;
        }
    };



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

        spNativeTexture _base;
        spNativeTexture _alpha;
        blend_mode      _blend;
        int             _flags;

        void init(size_t& hash);
        void apply();
        bool isSame(const STDMatData& b) const;
    };

    typedef intrusive_ptr< MaterialTX<STDMatData> > spSTDMaterialX;

    class MaterialCache
    {
    public:

        typedef std::vector<spMaterialX> materials;
        materials _materials;

        template<class T>
        intrusive_ptr< MaterialTX<T> > add(const T& other)
        {
            MaterialTX<T> mat(other);
            return add2(mat);
        }

        template<class T>
        intrusive_ptr<T> add2(const T& other)
        {
            for (auto m : _materials)
            {
                if (m->_compare != other._compare)
                    continue;
                if (m->_hash != other._hash)
                    continue;
                bool same = m->_compare(m.get(), &other);
                if (same)
                    return (T*)m.get();
            }

            T* copy = other.clone();
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