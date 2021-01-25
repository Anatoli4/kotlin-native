/*
 * Copyright 2010-2021 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "Freezing.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "FreezeHooksTestSupport.hpp"
#include "Memory.h"
#include "Utils.hpp"

using namespace kotlin;

namespace {

struct WithFreezeHook {
    static constexpr bool hasFreezeHook = true;
};

struct NoFreezeHook {
    static constexpr bool hasFreezeHook = false;
};

template <size_t Fields, typename Traits = NoFreezeHook>
class Object : private Pinned {
public:
    Object() {
        auto* type = new TypeInfo();
        type->typeInfo_ = type;
        if (Traits::hasFreezeHook) {
            type->flags_ |= TF_HAS_FREEZE_HOOK;
        } else {
            type->flags_ &= ~TF_HAS_FREEZE_HOOK;
        }
        type->objOffsetsCount_ = Fields;
        if (Fields > 0) {
            auto* offsets = new int32_t[Fields];
            for (size_t i = 0; i < Fields; ++i) {
                offsets[i] = reinterpret_cast<uintptr_t>(&fields_[i]) - reinterpret_cast<uintptr_t>(&header_);
            }
            type->objOffsets_ = offsets;
        }
        header_.typeInfoOrMeta_ = type;
    }

    ~Object() {
        auto* type = header_.type_info();
        if (Fields > 0) {
            delete[] type->objOffsets_;
        }
        delete type;
    }

    ObjHeader* header() { return &header_; }

    ObjHeader*& operator[](size_t field) { return fields_[field]; }

private:
    ObjHeader header_;
    std::array<ObjHeader*, Fields> fields_;
};

template <size_t Elements, typename Traits = NoFreezeHook>
class Array : private Pinned {
public:
    Array() {
        auto* type = new TypeInfo();
        type->typeInfo_ = type;
        if (Traits::hasFreezeHook) {
            type->flags_ |= TF_HAS_FREEZE_HOOK;
        } else {
            type->flags_ &= ~TF_HAS_FREEZE_HOOK;
        }
        header_.typeInfoOrMeta_ = type;
        header_.count_ = Elements;
    }

    ~Array() {
        auto* type = header_.type_info();
        delete type;
    }

    ObjHeader* header() { return header_.obj(); }

    ObjHeader*& operator[](size_t index) { return elements_[index]; }

private:
    ArrayHeader header_;
    std::array<ObjHeader*, Elements> elements_;
};

class FreezingTest : public testing::Test {
public:
    testing::MockFunction<void(ObjHeader*)>& freezeHook() { return freezeHooks_.freezeHook(); }

private:
    FreezeHooksTestSupport freezeHooks_;
};

class TypesNames {
public:
    template <typename T>
    static std::string GetName(int i) {
        switch (i) {
            case 0: return "object";
            case 1: return "array";
            default: return "unknown";
        }
    }
};

template <typename T>
class FreezingEmptyNoHookTest : public FreezingTest {};
using EmptyNoHookTypes = testing::Types<Object<0, NoFreezeHook>, Array<0, NoFreezeHook>>;
TYPED_TEST_SUITE(FreezingEmptyNoHookTest, EmptyNoHookTypes, TypesNames);

template <typename T>
class FreezingEmptyWithHookTest : public FreezingTest {};
using EmptyWithHookTypes = testing::Types<Object<0, WithFreezeHook>, Array<0, WithFreezeHook>>;
TYPED_TEST_SUITE(FreezingEmptyWithHookTest, EmptyWithHookTypes, TypesNames);

template <typename T>
class FreezingNoHookTest : public FreezingTest {};
using NoHookTypes = testing::Types<Object<3, NoFreezeHook>, Array<3, NoFreezeHook>>;
TYPED_TEST_SUITE(FreezingNoHookTest, NoHookTypes, TypesNames);

template <typename T>
class FreezingWithHookTest : public FreezingTest {};
using WithHookTypes = testing::Types<Object<3, WithFreezeHook>, Array<3, WithFreezeHook>>;
TYPED_TEST_SUITE(FreezingWithHookTest, WithHookTypes, TypesNames);

} // namespace

TYPED_TEST(FreezingEmptyNoHookTest, UnfrozenByDefault) {
    TypeParam object;
    EXPECT_FALSE(mm::IsFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyNoHookTest, FailToEnsureNeverFrozen) {
    TypeParam object;
    ASSERT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    ASSERT_TRUE(mm::IsFrozen(object.header()));
    EXPECT_FALSE(mm::EnsureNeverFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyNoHookTest, Freeze) {
    TypeParam object;
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    EXPECT_TRUE(mm::IsFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyNoHookTest, FreezeTwice) {
    TypeParam object;
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    EXPECT_TRUE(mm::IsFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyNoHookTest, FreezeForbidden) {
    TypeParam object;
    ASSERT_TRUE(mm::EnsureNeverFrozen(object.header()));
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), object.header());
    EXPECT_FALSE(mm::IsFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyWithHookTest, Freeze) {
    TypeParam object;
    EXPECT_CALL(this->freezeHook(), Call(object.header()));
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    EXPECT_TRUE(mm::IsFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyWithHookTest, FreezeTwice) {
    TypeParam object;
    // Only called for the first freeze.
    EXPECT_CALL(this->freezeHook(), Call(object.header()));
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    testing::Mock::VerifyAndClearExpectations(&this->freezeHook());
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), nullptr);
    EXPECT_TRUE(mm::IsFrozen(object.header()));
}

TYPED_TEST(FreezingEmptyWithHookTest, FreezeForbidden) {
    TypeParam object;
    ASSERT_TRUE(mm::EnsureNeverFrozen(object.header()));
    EXPECT_CALL(this->freezeHook(), Call(object.header()));
    EXPECT_THAT(mm::FreezeSubgraph(object.header()), object.header());
    EXPECT_FALSE(mm::IsFrozen(object.header()));
}
