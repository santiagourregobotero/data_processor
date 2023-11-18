//          Copyright Santiago Urrego Botero 2022.



#include <iomanip>
#include <iostream>
#include "mainframe/detail/base.hpp"

using namespace std;

namespace mf::detail
{

const char*
get_horzrule(size_t num)
{
    static const char horzrule[] = "________________________________________"
                                   "________________________________________"
                                   "________________________________________"
                                   "________________________________________"
                                   "________________________________________"
                                   "________________________________________"
                                   "________________________________________"
                                   "________________________________________";
    auto horzrulesize            = sizeof(horzrule) - 1;
    num                          = std::min(num, horzrulesize);
    return horzrule + (horzrulesize - num);
}

const char*
get_emptyspace(size_t num)
{
    static const char emptyspace[] = "                                        "
                                     "                                        "
                                     "                                        "
                                     "                                        "
                                     "                                        "
                                     "                                        "
                                     "                                        "
                                     "                                        ";
    auto emptyspacesize            = sizeof(emptyspace) - 1;
    num                            = std::min(num, emptyspacesize);
    return emptyspace + (emptyspacesize - num);
}

size_t
get_max_string_length(const std::vector<std::string>& s)
{
    size_t max_len = 0;
    for (const std::string& rowelem : s) {
        max_len = std::max(max_len, rowelem.size());
    }
    return max_len;
}

std::vector<size_t>
get_max_string_lengths(const std::vector<std::vector<std::string>>& columns)
{
    std::vector<size_t> out;
    for (const std::vector<std::string>& col : columns) {
        out.push_back(get_max_string_length(col));
    }
    return out;
}

std::ostream&
stringify(std::ostream& o, const char& t, bool)
{
    o << static_cast<int>(t);
    return o;
}

std::ostream&
stringify(std::ostream& o, const unsigned char& t, bool)
{
    o << static_cast<int>(t);
    return o;
}

} // namespace mf::detail

