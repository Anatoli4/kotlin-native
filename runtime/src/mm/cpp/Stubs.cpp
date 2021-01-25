/*
 * Copyright 2010-2020 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "Memory.h"

#include "KAssert.h"

ALWAYS_INLINE bool isFrozen(const ObjHeader* obj) {
    TODO();
}

ALWAYS_INLINE bool isPermanentOrFrozen(const ObjHeader* obj) {
    TODO();
}

extern "C" {

void MutationCheck(ObjHeader* obj) {
    TODO();
}

void FreezeSubgraph(ObjHeader* obj) {
    TODO();
}

void EnsureNeverFrozen(ObjHeader* obj) {
    TODO();
}

RUNTIME_NOTHROW void PerformFullGC(MemoryState* memory) {
    TODO();
}

ForeignRefContext InitLocalForeignRef(ObjHeader* object) {
    TODO();
}

RUNTIME_NOTHROW void Kotlin_mm_safePointFunctionEpilogue() {
    TODO();
}

RUNTIME_NOTHROW void Kotlin_mm_safePointWhileLoopBody() {
    TODO();
}

RUNTIME_NOTHROW void Kotlin_mm_safePointExceptionUnwind() {
    TODO();
}

} // extern "C"
