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

int split(std::vector<std::string>& result, const std::string& str, const std::string& delimiter);
