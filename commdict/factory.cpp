#include "factory.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <google/protobuf/descriptor.h>

#include "common.h"
#include "staff.pb.h"
#include "student.pb.h"

/* init static var */
std::unordered_map<std::string, parse_function> Factory::_func_map;

/* define custom parse function */
int parse_address(const std::string& line, google::protobuf::Message* msg) {
    Address* addr = static_cast<Address*>(msg);

    std::vector<std::string> vs;

    if (split(vs, line, "|") <= 0) {
        return -1;
    }

    if (vs.size() < 3) {
        return -1;
    }

    addr->set_city(vs[0]);
    addr->set_street(vs[1]);
    addr->set_building(vs[2]);
    return 0;
}

int parse_prov(const std::string& line, google::protobuf::Message* msg) {
    Prov* p = static_cast<Prov*>(msg);
    LOG(INFO)<< "line:"<< line;
    std::vector<std::string> vs;

    if (split(vs, line, "|") <= 0) {
        return -1;
    }

    if (vs.size() < 2) {
        return -1;
    }

    try {
        int id = stoi(vs[0]);
        p->set_id(id);
    } catch (...) {
        LOG(ERROR)<< "invalid id:"<< vs[0];
        return -1;
    }

    LOG(INFO)<< "name:%s", vs[1];
    p->set_name(vs[1]);
    return 0;
}

// 将函数指针注册到工厂类里的map中，便于解析词表自定义类型时候通过字符串名找到解析函数
Factory::Register register_address("Address", parse_address);
Factory::Register register_prov("Prov", parse_prov);

