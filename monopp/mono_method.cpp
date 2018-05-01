#include "mono_method.h"
#include "mono_object.h"

#include <utility>

namespace mono
{

mono_method::mono_method(mono_assembly* assembly, MonoClass* cls, MonoObject* object, const std::string& name,
						 int argc)
	: assembly_(assembly)
    , object_(object)
{
	method_ = mono_class_get_method_from_name(cls, name.c_str(), argc);
}

mono_method::mono_method(mono_method&& o) = default;

auto mono_method::operator=(mono_method&& o) -> mono_method& = default;

} // namespace mono
