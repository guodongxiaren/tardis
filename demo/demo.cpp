// @filename:   demo.cpp
// @author:     guodongxiaren
// @biref:      common_dict demo
// @date:       2021-07-18

#include <libgen.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <string>

#include <glog/logging.h>
#include "tardis/dict.h"
#include "student.pb.h"
#include "userbid.pb.h"

int main(int argc, char** argv) {
    // 从配置文件load配置，并打开日志
    // 在主线程使用
    char buf[1024] = {0};
    getcwd(buf, 1024);
    std::string path(buf);
    std::cout << path << std::endl;
    google::InitGoogleLogging(argv[0]);


    std::string dict_name = path + "/data/student.dict";
    auto cd = tardis::Dict<Student>::get_instance(dict_name);
    auto student = cd->get_record_by_index(1);
    std::cout << student->name() << std::endl;
    std::cout << student->hobby(0) << std::endl;
    std::cout << student->addr().city() << std::endl;

    
    dict_name = path + "/data/userbid.dict";
    auto cub = tardis::Dict<UserBid>::get_instance(dict_name);
    auto ub = cub->get_record_by_key(101, "租房");
    std::cout << ub->bidword() << std::endl;

    google::ShutdownGoogleLogging();

    return 0;
}
