#pragma once
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

template <template <typename T> class C>
inline std::string join(const C<std::string>& vec, const char* delim) {
    std::stringstream res;
    std::copy(vec.begin(), vec.end(),
              std::ostream_iterator<std::string>(res, delim));
    return res.str();
}
