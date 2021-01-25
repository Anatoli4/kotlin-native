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

void (*g_hookOverrideForTesting)(ObjHeader*) = nullptr;

} // namespace

void kotlin::RunFreezeHooks(ObjHeader* object) noexcept {
    if (g_hookOverrideForTesting != nullptr) {
        g_hookOverrideForTesting(object);
        return;
    }
    // TODO: Consider some global registration.
    auto* type = object->type_info();
    if (type == theWorkerBoundReferenceTypeInfo) {
        WorkerBoundReferenceFreezeHook(object);
    }
}

void SetFreezeHookForTesting(void (*hook)(ObjHeader*)) noexcept {
    g_hookOverrideForTesting = hook;
}
