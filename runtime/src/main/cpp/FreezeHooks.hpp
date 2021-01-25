/*
 * Copyright 2010-2021 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#ifndef RUNTIME_MM_FREEZE_HOOKS_H
#define RUNTIME_MM_FREEZE_HOOKS_H

#include "Common.h"

struct ObjHeader;
struct TypeInfo;

namespace kotlin {

class FreezeHook {
public:
    using Hook = void(ObjHeader*);

    FreezeHook(const TypeInfo* type, Hook* hook) noexcept;

    const TypeInfo* type() const { return type_; }

    void operator()(ObjHeader* object) noexcept { hook_(object); }

private:
    const TypeInfo* const type_;
    Hook* const hook_;
};

#define INSTALL_FREEZE_HOOK(type, hook) \
    static __attribute__((used)) kotlin::FreezeHook g_freezeHook##type(type, hook)

// These hooks are only allowed to modify `object` subgraph.
void RunFreezeHooks(ObjHeader* object) noexcept;

} // namespace kotlin

#endif // RUNTIME_MM_FREEZE_HOOKS_H
