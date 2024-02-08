#pragma once
#include <string>

#include <gtest/gtest.h>

#define private public
#include "tardis/common.h"

class TestCommon : public ::testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

public:
    TestCommon(){};
    ~TestCommon(){};

public:
    std::string _cur_path;
};
