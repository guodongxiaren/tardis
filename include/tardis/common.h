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
    char* save = nullptr;
    // string to non-const char*
    char buf[MAX_BUF_SIZE] = {0};
    int ret = snprintf(buf, sizeof(buf), "%s", str.c_str());

    if (ret < 0) {
        return -1;
    }

    char* token = strtok_r(buf, delimiter.c_str(), &save);

    while (token != nullptr) {
        result.emplace_back(token);
        token = strtok_r(nullptr, delimiter.c_str(), &save);
    }

    return result.size();
}
