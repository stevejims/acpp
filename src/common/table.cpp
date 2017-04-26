#include "table.hpp"

table::table()
{
    data_["one"] = "foo";
    data_["two"] = "bar";
}

std::experimental::string_view table::lookup(const std::string &key) const
{
    return data_.at(key);
}
