#pragma once

#include "mono_config.h"
#include <functional>
namespace mono
{
using log_handler = std::function<void(const std::string&)>;
void set_log_handler(const std::string& category, const log_handler& handler);
auto get_log_handler(const std::string& category) -> const log_handler&;
} // namespace mono
