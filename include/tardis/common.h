// @filename:   common.h
// @author:     guodongxiaren
// @biref:      common function lib
// @date:       2021-07-18
#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// const
const int MAX_BUF_SIZE = 1024;

template<typename T>
std::string join_param(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template<typename T, typename ...Args>
std::string join_param(const T& value, Args... args) {
    std::stringstream ss;
    ss << value << "\t" << join_param(args...);
    return ss.str();
}

inline int split(std::vector<std::string>& result, const std::string& str, const std::string& delimiter) {
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
