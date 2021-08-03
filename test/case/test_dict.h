#pragma once
#include <memory>
#include <string>
#include <gtest/gtest.h>

#include "staff.pb.h"
#define private public
#include "tardis/dict.h"

extern char STAFF[];
class TestDict : public ::testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();
public:
    TestDict() {
    };
    ~TestDict() {
    };
public:
    std::string _cur_path;
    tardis::Dict<Staff, STAFF>* _cd_staff;
};
