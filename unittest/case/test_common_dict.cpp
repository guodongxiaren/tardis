#include "test_common_dict.h"

#include <libgen.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "staff.pb.h"

using std::string;
using std::vector;
void TestCommonDict::SetUp() {
    char buf[1024] = {0};
    getcwd(buf, 1024);
    _cur_path.assign(dirname(buf));
    cout << _cur_path << endl;

    google::InitGoogleLogging(NULL);

    string dict_filename = _cur_path + "/data/staff.dict";
    _cd_staff = CommonDict<Staff>::get_instance(dict_filename);
}

void TestCommonDict::TearDown() {
    google::ShutdownGoogleLogging();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// 创建CommonDict对象
TEST_F(TestCommonDict, new_CommonDict) {
    string dict_filename = _cur_path + "/data/staff.dict";
    auto cd = new(std::nothrow)CommonDict<Staff>(dict_filename);
    ASSERT_FALSE(cd == nullptr);
    ASSERT_EQ(cd->_dict_filename, dict_filename);
}

// 正常加载文件(load_file、read_line)
TEST_F(TestCommonDict, load_file) {
    string dict_filename = _cur_path + "/data/staff.dict";
    auto cd = new(std::nothrow)CommonDict<Staff>(dict_filename);
    ASSERT_FALSE(cd == nullptr);
    ASSERT_EQ(cd->_dict_filename, dict_filename);
    int ret = cd->load_file();
    ASSERT_EQ(ret, 0);
}

// 加载格式非法的文件
TEST_F(TestCommonDict, load_file_wrong_format) {
    string dict_filename = _cur_path + "/data/staff_err.dict";
    LOG(INFO)<< "wrong dict path:%s"<< dict_filename;
    auto cd = new(std::nothrow)CommonDict<Staff>(dict_filename);
    ASSERT_FALSE(cd == nullptr);
    ASSERT_EQ(cd->_dict_filename, dict_filename);
    int ret = cd->load_file();
    ASSERT_NE(ret, 0);
}

// 加载路径不合法的文件
TEST_F(TestCommonDict, get_instance_no_dictfilename) {
    auto cd = CommonDict<Staff>::get_instance("");
    ASSERT_TRUE(cd == nullptr);
}

// 测试get_instance类似load_file
TEST_F(TestCommonDict, get_instance_dictfilename) {
    string dict_filename = _cur_path + "/data/staff.dict";
    LOG(INFO)<< "dict path:"<< dict_filename;
    auto cd = CommonDict<Staff>::get_instance(dict_filename);
    ASSERT_FALSE(cd == nullptr);
}

// 测试get_record_by_key 有效的id;词表文件中id有1 2 3
TEST_F(TestCommonDict, get_record_by_key) {
    auto staff = _cd_staff->get_record_by_key(1);
    ASSERT_EQ(staff->name(), "wangwei");
}

// 测试get_record_by_key 不存在的id;词表文件中id有1 2 3
TEST_F(TestCommonDict, get_record_by_key_invalid_id) {
    auto staff = _cd_staff->get_record_by_key(0);
    ASSERT_EQ(staff, nullptr);
}

// 测试get_record_by_index
TEST_F(TestCommonDict, get_record_by_index) {
    auto staff = _cd_staff->get_record_by_index(0);
    ASSERT_NE(staff, nullptr);
}

// 测试get_record_by_index 超出下标范围
TEST_F(TestCommonDict, get_record_by_index_invalid_index) {
    auto staff = _cd_staff->get_record_by_index(100000);
    ASSERT_EQ(staff, nullptr);
}
