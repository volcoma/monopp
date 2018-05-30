#pragma once

#include "mono_config.h"
#include <functional>
namespace mono
{
using log_handler = std::function<void(const std::string&)>;
void set_log_handler(const std::string& category, const log_handler& handler);
const log_handler& get_log_handler(const std::string& category);
} // namespace mono
