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

struct HasFreezeHook {
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

} // namespace

TEST_F(FreezingTest, EmptyObjectNoHook) {
    Object<0> object;
    mm::FreezeSubgraph(object.header());
    EXPECT_TRUE(mm::IsFrozen(object.header()));
}

TEST_F(FreezingTest, EmptyObjectWithHook) {
    Object<0, HasFreezeHook> object;
    EXPECT_CALL(freezeHook(), Call(object.header()));
    mm::FreezeSubgraph(object.header());
    EXPECT_TRUE(mm::IsFrozen(object.header()));
}

TEST_F(FreezingTest, EmptyArrayNoHook) {
    Array<0> array;
    mm::FreezeSubgraph(array.header());
    EXPECT_TRUE(mm::IsFrozen(array.header()));
}

TEST_F(FreezingTest, EmptyArrayWithHook) {
    Array<0, HasFreezeHook> array;
    EXPECT_CALL(freezeHook(), Call(array.header()));
    mm::FreezeSubgraph(array.header());
    EXPECT_TRUE(mm::IsFrozen(array.header()));
}
