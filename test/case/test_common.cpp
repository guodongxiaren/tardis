#include "test_common.h"

#include <libgen.h>

#include <string>
#include <vector>

#include "staff.pb.h"

void TestCommon::SetUp() {

    char buf[1024] = {0};
    readlink("/proc/self/exe", buf, 1024);
    std::cout << buf << std::endl;
    _cur_path.assign(dirname(buf));
}

void TestCommon::TearDown() {
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    google::ShutdownGoogleLogging();

    return ret;
}
// 测试split
TEST_F(TestCommon, split) {
    std::string line = "a|b|c";
    std::vector<std::string> vs;
    int ret = split(vs, line, "|");

    ASSERT_EQ(ret, 3);
    ASSERT_EQ(vs.size(), 3);
    ASSERT_EQ(vs[0], "a");
    ASSERT_EQ(vs[1], "b");
    ASSERT_EQ(vs[2], "c");
}
// 测试split TAB分隔
TEST_F(TestCommon, split_t) {
    std::string line = "a\tb\tc";
    std::vector<std::string> vs;
    int ret = split(vs, line, "\t");
    ASSERT_EQ(ret, 3);
    ASSERT_EQ(vs.size(), 3);
    ASSERT_EQ(vs[0], "a");
    ASSERT_EQ(vs[1], "b");
    ASSERT_EQ(vs[2], "c");
}
