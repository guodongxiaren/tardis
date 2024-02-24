// @filename:   demo.cpp
// @author:     guodongxiaren
// @biref:      common_dict demo
// @date:       2021-07-18

#include <libgen.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <string>

#include "tardis/dict.h"
#include "student.pb.h"
#include "country.pb.h"

int main(int argc, char** argv) {

    // 从配置文件load配置，并打开日志
    // 在主线程使用
    char buf[1024] = {0};
    getcwd(buf, 1024);
    std::string path(buf);
    std::cout << path << std::endl;


    std::string dict_name = path + "/data/student.dict";
    auto& cd = tardis::Dict<Student>::Instance();
    cd.LoadFile(dict_name);
    auto student = cd.Search(1);
    if (student) {
        std::cout << student->name() << std::endl;
        std::cout << student->hobby(0) << std::endl;
        std::cout << student->addr().city() << std::endl;
    }

    
    dict_name = path + "/data/iso-3166.csv";
    auto& cub = tardis::Dict<Country>::Instance();
    cub.LoadFile(dict_name);
    auto country = cub.Search("China");
    std::cout << country->alpha_3() << std::endl;
    std::cout << country->code() << std::endl;

    return 0;
}

