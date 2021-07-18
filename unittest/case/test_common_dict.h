#pragma once
#define private public
#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "common_dict.h"
#include "staff.pb.h"

class TestCommonDict : public ::testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();
public:
    TestCommonDict() {
    };
    ~TestCommonDict() {
    };
public:
    std::string _cur_path;
    std::shared_ptr<CommonDict<Staff>> _cd_staff;
};
