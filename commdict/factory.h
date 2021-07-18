// @filename:   factory.h
// @author:     guodongxiaren
// @biref:      parse factory of custom type
// @date:       2021-07-18
#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <glog/logging.h>
#include <google/protobuf/descriptor.h>

using parse_function = std::function<int(const std::string&, google::protobuf::Message*)>;

class Factory {
public:
    struct Register {
        Register(const std::string& key, parse_function func) {
            Factory::set(key, func);
        }
    };

    static void set(const std::string& key, parse_function func) {
        _func_map.emplace(key, func);
    }

    static parse_function get(const std::string& key) {
        if (_func_map.find(key) == _func_map.end()) {
            LOG(ERROR)<< "message key:" << key <<" is not exist";
            return nullptr;
        }

        return _func_map[key];
    }

private:
    Factory() {};
    Factory(const Factory&) = delete;
    Factory(Factory&&) = delete;
    static std::unordered_map<std::string, parse_function> _func_map;
};

