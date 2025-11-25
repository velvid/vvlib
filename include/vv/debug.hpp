#pragma once

#include <cassert>

#if !defined(DBG)
#if defined(_DEBUG)
#define DBG 1
#endif
#endif

#ifdef DBG
#define VV_DEBUG_VAR_INIT(T, name, value) T name = value
#define VV_DEBUG_VAR_ASSIGN(name, value) name = value
#else
#define VV_DEBUG_VAR_INIT(T, name, value)
#define VV_DEBUG_VAR_ASSIGN(name, value)
#endif

#ifdef DBG
#define VV_PANIC() std::abort()
#define VV_PANIC_IF(cond)                                                                          \
    if (cond) std::abort()
#define VV_PANIC_IF_NOT(cond)                                                                      \
    if (!cond) std::abort()
#else
#define VV_PANIC
#define VV_PANIC_IF
#define VV_PANIC_IF_NOT
#endif
