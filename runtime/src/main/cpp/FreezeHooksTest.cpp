/*
 * Copyright 2010-2021 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "FreezeHooks.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Memory.h"
#include "TypeInfo.h"

using namespace kotlin;

namespace {

TypeInfo type1;
TypeInfo type2;
TypeInfo type3;  // Not registered as freeze hook.

const TypeInfo* type1Ptr = &type1;
const TypeInfo* type2Ptr = &type2;

testing::StrictMock<testing::MockFunction<void(ObjHeader*)>> mock1;
testing::StrictMock<testing::MockFunction<void(ObjHeader*)>> mock2;

void hook1(ObjHeader* object) noexcept {
    mock1.Call(object);
}

void hook2(ObjHeader* object) noexcept {
    mock2.Call(object);
}

INSTALL_FREEZE_HOOK(type1Ptr, hook1);
INSTALL_FREEZE_HOOK(type2Ptr, hook2);

ObjHeader obj1 = { &type1 };
ObjHeader obj2 = { &type2 };
ObjHeader obj3 = { &type3 };

} // namespace

TEST(FreezeHooksTest, Basic) {
    EXPECT_CALL(mock1, Call(&obj1));
    RunFreezeHooks(&obj1);
    testing::Mock::VerifyAndClearExpectations(&mock1);
    testing::Mock::VerifyAndClearExpectations(&mock2);

    EXPECT_CALL(mock2, Call(&obj2));
    RunFreezeHooks(&obj2);
    testing::Mock::VerifyAndClearExpectations(&mock1);
    testing::Mock::VerifyAndClearExpectations(&mock2);

    RunFreezeHooks(&obj3);
    testing::Mock::VerifyAndClearExpectations(&mock1);
    testing::Mock::VerifyAndClearExpectations(&mock2);

    testing::Mock::VerifyAndClear(&mock1);
    testing::Mock::VerifyAndClear(&mock2);
}
