#pragma once

#include "mono_config.h"

#include "mono_assembly.h"

namespace mono
{

auto init(const std::string& domain, bool enable_debugging = false) -> bool;

void shutdown();

} // namespace mono
