#include "test_dict.h"

#include <libgen.h>
#include <string>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>
#include "staff.pb.h"
#include "tardis/common.h"

#include <unistd.h>
#include <linux/limits.h>


using std::string;
using std::vector;
char STAFF[] = "staff";
void TestDict::SetUp() {
	char dir[PATH_MAX] = {0};
    readlink("/proc/self/exe", dir, PATH_MAX);

    std::vector<std::string> vec;
    tardis::split(vec, dir, "/");
    std::stringstream ss;
    for (int i = 0; i < vec.size() - 1; ++i) {
        ss << '/' << vec[i]; 
    }

    _cur_path.assign(ss.str());
    std::cout << _cur_path << std::endl;

    string dict_filename = _cur_path + "/data/staff.dict";

    std::cout << dict_filename << std::endl;

    _cd_staff = &tardis::Dict<Staff, STAFF>::Instance();
    _cd_staff->LoadFile(dict_filename);
}

void TestDict::TearDown() {
}

int main(int argc, char** argv) {

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}

// 正常加载文件(LoadFile、read_line)
TEST_F(TestDict, LoadFile) {
    string dict_filename = _cur_path + "/data/staff.dict";
    auto& cd = tardis::Dict<Staff, STAFF>::Instance();
    int ret = cd.LoadFile(dict_filename);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(cd._dict_filename, dict_filename);
}

// 加载格式非法的文件
TEST_F(TestDict, LoadFile_wrong_format) {
    string dict_filename = _cur_path + "/data/staff_err.dict";
    std::cout<< "wrong dict path:%s"<< dict_filename;
    auto& cd = tardis::Dict<Staff, STAFF>::Instance();
    int ret = cd.LoadFile(dict_filename);
    ASSERT_NE(ret, 0);
    ASSERT_EQ(cd._dict_filename, dict_filename);
}

// 加载路径不合法的文件
TEST_F(TestDict, Instance_no_dictfilename) {
    int ret = tardis::Dict<Staff, STAFF>::Instance().LoadFile("");
    ASSERT_FALSE(ret == 0);
}

// 测试Instance类似LoadFile
TEST_F(TestDict, Instance_dictfilename) {
    string dict_filename = _cur_path + "/data/staff.dict";
    std::cout<< "dict path:"<< dict_filename;
    int ret = tardis::Dict<Staff, STAFF>::Instance().LoadFile(dict_filename);
    ASSERT_TRUE(ret == 0);
}

// 测试Search 有效的id;词表文件中id有1 2 3
TEST_F(TestDict, Search) {
    auto staff = _cd_staff->Search(1);
    ASSERT_EQ(staff->name(), "wangwei");
}

// 测试Search 不存在的id;词表文件中id有1 2 3
TEST_F(TestDict, Search_invalid_id) {
    auto staff = _cd_staff->Search(0);
    ASSERT_EQ(staff, nullptr);
}
