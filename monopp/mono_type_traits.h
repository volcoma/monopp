#pragma once

#include "common/nonstd/traits/function_traits.hpp"

namespace mono
{

template <typename T>
using is_mono_valuetype = std::is_pod<T>;

template <typename T>
using function_traits = nonstd::function_traits<T>;
}
