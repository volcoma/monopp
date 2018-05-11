#pragma once

#include <cstdint>
#include <string>
#include <cassert>
#include <stdexcept>

#include "common/platform/export.hpp"

#if ETH_ON(ETH_PLATFORM_WINDOWS)
#ifndef MONO_DLL_IMPORT
#define MONO_DLL_IMPORT
#endif
#endif
