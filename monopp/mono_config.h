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
