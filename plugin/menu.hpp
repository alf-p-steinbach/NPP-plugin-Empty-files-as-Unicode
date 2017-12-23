#pragma once
#include <notepad++/plugin-dll-interface.hpp>       // ::FuncItem
#include <stdlib/extension/type_builders.hpp>       // ptr_

namespace menu{
    using Item     = ::FuncItem;
    using namespace stdlib::type_builders;

    struct Static_cmd_id
    {
        enum Enum: int { set_enabled = 2, set_disabled = 3 };
    };

    struct Items
    {
        ptr_<Item>  item;                           // Item array
        int         count;
    };

    auto items() -> Items;

    inline auto dynamic_cmd_id( const Static_cmd_id::Enum i )
        -> int
    { return items().item[i]._cmdID; }

}  // menu
