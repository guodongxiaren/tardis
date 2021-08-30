// @filename:   common.h
// @author:     guodongxiaren
// @biref:      common function lib
// @date:       2021-07-18
#pragma once
#include <stdint.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace tardis {

template <typename T> std::string join_param(const T &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template <typename T, typename... Args>
std::string join_param(const T &value, Args... args) {
    std::stringstream ss;
    ss << value << "\t" << join_param(args...);
    return ss.str();
}

inline int split(std::vector<std::string> &result, const std::string &str,
                 const std::string &delimiter) {
    result.clear();
    int start = 0;
    while (start < str.size()) {
        int end = str.find(delimiter, start);
        if (end == std::string::npos) {
            result.emplace_back(str, start);
            break;
        }
        if (end > start) {
            result.emplace_back(str, start, end - start);
        }
        start = end + 1;
    }
    return result.size();
}

// common cast, but it's slow
template <typename T> T lexical_cast(const std::string &str) {
    T var;
    std::istringstream iss;
    iss.str(str);
    iss >> var;
    return var;
}

template <> bool lexical_cast<bool>(const std::string &str) {
    if (str == "true" || str == "True") {
        return true;
    } else if (str == "false" || str == "False") {
        return false;
    }
    return std::stoi(str) != 0;
}

template <> int32_t lexical_cast<int32_t>(const std::string &str) {
    return std::stoi(str);
}

} // namespace tardis
