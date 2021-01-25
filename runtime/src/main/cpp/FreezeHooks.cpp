/*
 * Copyright 2010-2021 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "FreezeHooks.hpp"

#include "Memory.h"
#include "Types.h"
#include "WorkerBoundReference.h"

using namespace kotlin;

namespace {

std::vector<FreezeHook*> g_hooks;

}  // namespace

kotlin::FreezeHook::FreezeHook(const TypeInfo* type, Hook* hook) noexcept : type_(type), hook_(hook) {
    g_hooks.push_back(this);
}

void kotlin::RunFreezeHooks(ObjHeader* object) noexcept {
    auto* type = object->type_info();
    for (FreezeHook* hook : g_hooks) {
        if (hook->type() == type) {
            (*hook)(object);
            return;
        }
    }
}
