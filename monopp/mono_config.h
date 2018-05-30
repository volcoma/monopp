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

#ifdef _MSC_VER

#define BEGIN_MONO_INCLUDE \
__pragma(warning(push)) \
__pragma(warning(disable : 4201))
    
#define END_MONO_INCLUDE __pragma(warning(pop))
#else
#define BEGIN_MONO_INCLUDE
#define END_MONO_INCLUDE
#endif
