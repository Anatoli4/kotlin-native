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

OBJ_GETTER(InitThreadLocalSingleton, ObjHeader** location, const TypeInfo* typeInfo, void (*ctor)(ObjHeader*)) {
    TODO();
}


RUNTIME_NOTHROW OBJ_GETTER(
        SwapHeapRefLocked, ObjHeader** location, ObjHeader* expectedValue, ObjHeader* newValue, int32_t* spinlock, int32_t* cookie) {
    TODO();
}

RUNTIME_NOTHROW void SetHeapRefLocked(ObjHeader** location, ObjHeader* newValue, int32_t* spinlock, int32_t* cookie) {
    TODO();
}

RUNTIME_NOTHROW OBJ_GETTER(ReadHeapRefLocked, ObjHeader** location, int32_t* spinlock, int32_t* cookie) {
    TODO();
}

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

bool TryAddHeapRef(const ObjHeader* object) {
    TODO();
}

RUNTIME_NOTHROW void ReleaseHeapRef(const ObjHeader* object) {
    TODO();
}

RUNTIME_NOTHROW void ReleaseHeapRefNoCollect(const ObjHeader* object) {
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

RUNTIME_NOTHROW void Kotlin_mm_safePointExternalCallReturn() {
    TODO();
}

RUNTIME_NOTHROW void Kotlin_mm_safePointExternalCallUnwind() {
    TODO();
}

} // extern "C"
