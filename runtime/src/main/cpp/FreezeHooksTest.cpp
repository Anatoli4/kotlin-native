/*
 * Copyright 2010-2021 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "FreezeHooks.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Memory.h"

using namespace kotlin;

using ::testing::_;

namespace {

class FreezeHooksTest : public testing::Test {
public:
    FreezeHooksTest() {
        globalMockHook_ = &mockFreezeHook;
        SetFreezeHookForTesting(freezeHookImpl);
    }

    ~FreezeHooksTest() override {
        SetFreezeHookForTesting(nullptr);
        globalMockHook_ = nullptr;
    }

    testing::MockFunction<void(ObjHeader*)>& freezeHook() { return mockFreezeHook; }

private:
    testing::StrictMock<testing::MockFunction<void(ObjHeader*)>> mockFreezeHook;

    static testing::MockFunction<void(ObjHeader*)>* globalMockHook_;

    static void freezeHookImpl(ObjHeader* object) {
        globalMockHook_->Call(object);
    }

};

// static
testing::MockFunction<void(ObjHeader*)>* FreezeHooksTest::globalMockHook_ = nullptr;

} // namespace

TEST_F(FreezeHooksTest, TypeWithFreezeHook) {
    TypeInfo type;
    type.typeInfo_ = &type;
    type.flags_ |= TF_HAS_FREEZE_HOOK;
    ObjHeader obj = { &type };
    EXPECT_CALL(freezeHook(), Call(&obj));
    RunFreezeHooks(&obj);
    testing::Mock::VerifyAndClearExpectations(&freezeHook());
}

TEST_F(FreezeHooksTest, TypeWithoutFreezeHook) {
    TypeInfo type;
    type.typeInfo_ = &type;
    type.flags_ &= ~TF_HAS_FREEZE_HOOK;
    ObjHeader obj = { &type };
    EXPECT_CALL(freezeHook(), Call(_)).Times(0);
    RunFreezeHooks(&obj);
    testing::Mock::VerifyAndClearExpectations(&freezeHook());
}
