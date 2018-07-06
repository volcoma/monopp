#include "mono_exception.h"
#include "mono_object.h"
#include "mono_property_invoker.h"
#include "mono_type.h"

namespace mono
{

auto get_exception_info(MonoObject* ex) -> mono_thunk_exception::mono_exception_info
{
	auto obj = mono_object(ex);
	const auto& type = obj.get_type();
	const auto& exception_typename = type.get_fullname();

	auto message_prop = type.get_property("Message");
	auto mutable_msg_prop = make_property_invoker<std::string>(message_prop);

	auto stacktrace_prop = type.get_property("StackTrace");
	auto mutable_stack_prop = make_property_invoker<std::string>(stacktrace_prop);

	auto message = mutable_msg_prop.get_value(obj);
	auto stacktrace = mutable_stack_prop.get_value(obj);
	return {exception_typename, message, stacktrace};
}

mono_thunk_exception::mono_thunk_exception(MonoObject* ex)
	: mono_thunk_exception(get_exception_info(ex))
{
}

auto mono_thunk_exception::exception_typename() const -> const std::string&
{
	return info_.exception_typename;
}

auto mono_thunk_exception::message() const -> const std::string&
{
	return info_.message;
}

auto mono_thunk_exception::stacktrace() const -> const std::string&
{
	return info_.stacktrace;
}

mono_thunk_exception::mono_thunk_exception(const mono_exception_info& info)
	: mono_exception(info.exception_typename + "(" + info.message + ")\n" + info.stacktrace)
	, info_(info)
{
}

} // namespace mono
