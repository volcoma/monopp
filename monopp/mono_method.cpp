#include "mono_method.h"
#include "mono_object.h"
#include "mono_exception.h"

namespace mono
{

mono_method::mono_method(mono_assembly* assembly, MonoClass* cls, MonoObject* object, const std::string& name,
						 int argc)
	: assembly_(assembly)
	, object_(object)
{
	method_ = mono_class_get_method_from_name(cls, name.c_str(), argc);

	if(!method_)
	{
		std::string cls_name = mono_class_get_name(cls);
		throw mono_exception("NATIVE::Could not get method : " + name + " for class " + cls_name);
	}
}

auto mono_method::get_internal_ptr() const -> MonoMethod*
{
	return method_;
}

mono_method::mono_method(mono_method&& o) = default;

auto mono_method::operator=(mono_method&& o) -> mono_method& = default;

} // namespace mono
