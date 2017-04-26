#ifndef TABLE_HPP
#define TABLE_HPP

#include <unordered_map>
#include <experimental/string_view>

class table {
public:
    table();

    std::experimental::string_view lookup(const std::string &key) const;

private:
    std::unordered_map<std::string, std::string> data_;
};

#endif /* TABLE_HPP */
