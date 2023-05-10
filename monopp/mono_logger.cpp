#include "mono_logger.h"
#include <map>
namespace mono
{
namespace detail
{
auto get_log_handler() -> std::map<std::string, log_handler>&
{
    static std::map<std::string, log_handler> log_callbacks;
	return log_callbacks;
}
}

auto get_log_handler(const std::string& category) -> const log_handler&
{
	return detail::get_log_handler()[category];
}

void set_log_handler(const std::string& category, const log_handler& handler)
{
	detail::get_log_handler()[category] = handler;
}

} // namespace mono
