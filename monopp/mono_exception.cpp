#include "mono_exception.h"

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
	auto exception_obj = reinterpret_cast<MonoObject*>(ex);
	auto exception_class = mono_object_get_class(exception_obj);
	auto exception_type = mono_class_get_type(exception_class);
	std::string exception_typename = mono_type_get_name(exception_type);
	std::string message = __get_string_property("Message", exception_class, exception_obj);
	std::string stacktrace = __get_string_property("StackTrace", exception_class, exception_obj);
	return {exception_typename, message, stacktrace};
}

auto mono_thunk_exception::__get_string_property(const char* property_name, MonoClass* cls, MonoObject* obj)
	-> char*
{
	auto property = mono_class_get_property_from_name(cls, property_name);
	auto getter = mono_property_get_get_method(property);
	auto value = reinterpret_cast<MonoString*>(mono_runtime_invoke(getter, obj, nullptr, nullptr));
	return mono_string_to_utf8(value);
}

} // namespace mono
