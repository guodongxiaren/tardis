#include "common.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int split(std::vector<std::string>& result, const std::string& str, const std::string& delimiter) {
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
