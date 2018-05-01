#include "mono_static_function.h"
#include <utility>

namespace mono
{

mono_static_function::mono_static_function(mono_assembly* assembly, MonoClass* cls, const std::string& name,
										   int argc)
    : assembly_(assembly)
{
	method_ = mono_class_get_method_from_name(cls, name.c_str(), argc);
}

mono_static_function::mono_static_function(mono_static_function&& o) = default;

auto mono_static_function::operator=(mono_static_function&& o) -> mono_static_function& = default;

} // namespace mono
