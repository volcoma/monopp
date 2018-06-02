#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#define MONO_ZERO_LEN_ARRAY 1

template <typename T>
using non_owning_ptr = T*;

#define DIAG_STR(s) #s
#define DIAG_JOINSTR(x, y) DIAG_STR(x##y)
#ifdef _MSC_VER
#define DIAG_DO_PRAGMA(x) __pragma(x)
#define DIAG_PRAGMA(compiler, x) DIAG_DO_PRAGMA(warning(x))
#else
#define DIAG_DO_PRAGMA(x) _Pragma(#x)
#define DIAG_PRAGMA(compiler, x) DIAG_DO_PRAGMA(compiler diagnostic x)
#endif
#if defined(__clang__)
#define PUSH_DISABLE_WARNING(gcc_unused, clang_option, msvc_unused)                                          \
	DIAG_PRAGMA(clang, push) DIAG_PRAGMA(clang, ignored DIAG_JOINSTR(-W, clang_option))
#define POP_DISABLE_WARNING() DIAG_PRAGMA(clang, pop)
#elif defined(_MSC_VER)
#define PUSH_DISABLE_WARNING(gcc_unused, clang_unused, msvc_errorcode)                                       \
	DIAG_PRAGMA(msvc, push) DIAG_PRAGMA(msvc, disable:##msvc_errorcode)
#define POP_DISABLE_WARNING() DIAG_PRAGMA(msvc, pop)
#elif defined(__GNUC__)
#define PUSH_DISABLE_WARNING(gcc_option, clang_unused, msvc_unused)                                          \
	DIAG_PRAGMA(GCC, push) DIAG_PRAGMA(GCC, ignored DIAG_JOINSTR(-W, gcc_option))
#define POP_DISABLE_WARNING() DIAG_PRAGMA(GCC, pop)
#endif

#define BEGIN_MONO_INCLUDE PUSH_DISABLE_WARNING(pedantic, pedantic, 4201)
#define END_MONO_INCLUDE POP_DISABLE_WARNING()
