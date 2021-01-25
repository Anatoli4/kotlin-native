/*
 * Copyright 2010-2021 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "FreezeHooks.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Memory.h"

using namespace kotlin;

namespace {

testing::StrictMock<testing::MockFunction<void(ObjHeader*)>> mockFreezeHook;

void freezeHook(ObjHeader* object) {
    mockFreezeHook.Call(object);
}

} // namespace

class FreezeHooksTest : public testing::Test {
public:
    void SetUp() override { SetFreezeHookForTesting(freezeHook); }

    void TearDown() override {
        SetFreezeHookForTesting(nullptr);
        testing::Mock::VerifyAndClear(&mockFreezeHook);
    }
};

TEST_F(FreezeHooksTest, Basic) {
    ObjHeader obj;
    EXPECT_CALL(mockFreezeHook, Call(&obj));
    RunFreezeHooks(&obj);
}
