#include "test_common.h"

#include <libgen.h>

#include <string>
#include <vector>

#include "staff.pb.h"

using std::string;
using std::vector;
void TestCommon::SetUp() {
    char buf[1024] = {0};
    readlink("/proc/self/exe", buf, 1024);
    cout << buf << endl;
    _cur_path.assign(dirname(buf));

    google::InitGoogleLogging(NULL);

}

void TestCommon::TearDown() {
    google::ShutdownGoogleLogging();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
// 测试split
TEST_F(TestCommon, split) {
    string line = "a|b|c";
    vector<string> vs;
    int ret = split(vs, line, "|");

    ASSERT_EQ(ret, 3);
    ASSERT_EQ(vs.size(), 3);
    ASSERT_EQ(vs[0], "a");
    ASSERT_EQ(vs[1], "b");
    ASSERT_EQ(vs[2], "c");
}
// 测试split TAB分隔
TEST_F(TestCommon, split_t) {
    string line = "a\tb\tc";
    vector<string> vs;
    int ret = split(vs, line, "\t");
    ASSERT_EQ(ret, 3);
    ASSERT_EQ(vs.size(), 3);
    ASSERT_EQ(vs[0], "a");
    ASSERT_EQ(vs[1], "b");
    ASSERT_EQ(vs[2], "c");
}
