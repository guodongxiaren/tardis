#pragma once
#define private public
#include <string>

#include "gtest/gtest.h"

#include <glog/logging.h>
#include "common.h"

class TestCommon : public ::testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();
public:
    TestCommon() {
    };
    ~TestCommon() {
    };
public:
    std::string _cur_path;
};
