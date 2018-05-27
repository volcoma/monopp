#include "mono_exception.h"
#include "mono_object.h"
#include "mono_property.h"
#include "mono_type.h"

namespace mono
{

mono_exception::mono_exception()
	: mono_exception("Mono Exception.")
{
}

mono_exception::mono_exception(const std::string& what)
	: std::runtime_error(what.c_str())
{
}

mono_thunk_exception::mono_thunk_exception(MonoException* ex)
	: mono_thunk_exception(__get_exception_info(ex))
{
}

mono_thunk_exception::mono_thunk_exception(const mono_exception_info& info)
	: mono_exception(info.exception_typename + "(" + info.message + ")\n" + info.stacktrace)
	, exception_typename_(info.exception_typename)
	, message_(info.message)
	, stacktrace_(info.stacktrace)
{
}

auto mono_thunk_exception::__get_exception_info(MonoException* ex) -> mono_exception_info
{
	auto obj = mono_object(reinterpret_cast<MonoObject*>(ex));
	auto type = obj.get_type();
	auto exception_typename = type.get_fullname();
	auto message_prop = type.get_property("Message");
	auto stacktrace_prop = type.get_property("StackTrace");
	auto message = message_prop.get_value<std::string>(obj);
	auto stacktrace = stacktrace_prop.get_value<std::string>(obj);
	return {exception_typename, message, stacktrace};
}

} // namespace mono
