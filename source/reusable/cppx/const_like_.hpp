#pragma once

#include <type_traits>          // std::remove_reference_t

namespace cppx{
    using std::remove_reference_t;

    namespace impl{
        template< class Other, class Type >
        struct With_const_as_t_
        {
            using T = Type;
        };

        template< class Other, class Type >
        struct With_const_as_t_<const Other, Type>
        {
            using T = const Type;
        };
    }  // namespace impl

    template< class Other, class Type >
    using const_like_ = typename impl::With_const_as_t_<
        remove_reference_t<Other>, Type
        >::T;

}  // namespace cppx
