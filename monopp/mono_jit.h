#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_type_conversion.h"

namespace mono
{

bool init(const std::string& domain, bool enable_debugging = false);

void shutdown();

} // namespace mono
