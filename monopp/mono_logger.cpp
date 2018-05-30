#include "mono_logger.h"
#include <map>
namespace mono
{
namespace detail
{
static std::map<std::string, log_handler> log_callbacks;
}

const log_handler& get_log_handler(const std::string& category)
{
	return detail::log_callbacks[category];
}

void set_log_handler(const std::string& category, const log_handler& handler)
{
	detail::log_callbacks[category] = handler;
}

} // namespace mono
