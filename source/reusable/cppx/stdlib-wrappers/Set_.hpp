#pragma once
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/unordered_set.hpp>             // std::unordered_set

namespace cppx{
    using std::unordered_set;
    using namespace stdlib::ext::type_builders;

    template< class Key >
    class Set_
        : public unordered_set<Key>
    {
    public:
        using unordered_set<Key>::unordered_set;
    };

    template< class Key, class Arg >
    auto in( ref_<const Set_<Key>> set, ref_<const Arg> v )
        -> bool
    { return set.count( v ) > 0; }

}  // namespace cppx
