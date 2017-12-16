#pragma once
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/unordered_map.hpp>             // std::unordered_map

namespace cppx{
    using std::unordered_map;
    using namespace stdlib::type_builders;

    template< class Key, class Value >
    class Map_
        : public unordered_map<Key, Value>
    {
    public:
        using unordered_map<Key, Value>::unordered_map;
    };

    template< class Key, class Value, class Arg >
    auto in( ref_<const Map_<Key, Value>> map, ref_<const Arg> v )
        -> bool
    { return map.count( v ) > 0; }

}  // namespace cppx
