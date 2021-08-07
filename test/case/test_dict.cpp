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
    split(vec, dir, "/");
    std::stringstream ss;
    for (int i = 0; i < vec.size() - 1; ++i) {
        ss << '/' << vec[i]; 
    }

    _cur_path.assign(ss.str());
    std::cout << _cur_path << std::endl;

    string dict_filename = _cur_path + "/data/staff.dict";

    std::cout << dict_filename << std::endl;

    _cd_staff = &tardis::Dict<Staff, STAFF>::get_instance();
    _cd_staff->load_file(dict_filename);
}

void TestDict::TearDown() {
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    google::ShutdownGoogleLogging();

    return ret;
}

// 正常加载文件(load_file、read_line)
TEST_F(TestDict, load_file) {
    string dict_filename = _cur_path + "/data/staff.dict";
    auto& cd = tardis::Dict<Staff, STAFF>::get_instance();
    int ret = cd.load_file(dict_filename);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(cd._dict_filename, dict_filename);
}

// 加载格式非法的文件
TEST_F(TestDict, load_file_wrong_format) {
    string dict_filename = _cur_path + "/data/staff_err.dict";
    LOG(INFO)<< "wrong dict path:%s"<< dict_filename;
    auto& cd = tardis::Dict<Staff, STAFF>::get_instance();
    int ret = cd.load_file(dict_filename);
    ASSERT_NE(ret, 0);
    ASSERT_EQ(cd._dict_filename, dict_filename);
}

// 加载路径不合法的文件
TEST_F(TestDict, get_instance_no_dictfilename) {
    int ret = tardis::Dict<Staff, STAFF>::get_instance().load_file("");
    ASSERT_FALSE(ret == 0);
}

// 测试get_instance类似load_file
TEST_F(TestDict, get_instance_dictfilename) {
    string dict_filename = _cur_path + "/data/staff.dict";
    LOG(INFO)<< "dict path:"<< dict_filename;
    int ret = tardis::Dict<Staff, STAFF>::get_instance().load_file(dict_filename);
    ASSERT_TRUE(ret == 0);
}

// 测试get_record_by_key 有效的id;词表文件中id有1 2 3
TEST_F(TestDict, get_record_by_key) {
    auto staff = _cd_staff->get_record_by_key(1);
    ASSERT_EQ(staff->name(), "wangwei");
}

// 测试get_record_by_key 不存在的id;词表文件中id有1 2 3
TEST_F(TestDict, get_record_by_key_invalid_id) {
    auto staff = _cd_staff->get_record_by_key(0);
    ASSERT_EQ(staff, nullptr);
}

// 测试get_record_by_index
TEST_F(TestDict, get_record_by_index) {
    auto staff = _cd_staff->get_record_by_index(0);
    ASSERT_NE(staff, nullptr);
}

// 测试get_record_by_index 超出下标范围
TEST_F(TestDict, get_record_by_index_invalid_index) {
    auto staff = _cd_staff->get_record_by_index(100000);
    ASSERT_EQ(staff, nullptr);
}
