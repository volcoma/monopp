#include "mono_class_property.h"
#include "mono_class.h"
#include <cassert>

namespace mono
{

mono_class_property::mono_class_property(const mono_class& monoclass, const std::string& name)
	: property_(mono_class_get_property_from_name(monoclass.get_internal_ptr(), name.c_str()))
{
	if(!property_)
		throw mono_exception("NATIVE::Could not get property : " + name + " for class " + monoclass.get_name());
}

mono_class_property::mono_class_property(mono_class_property&& o) = default;

auto mono_class_property::operator=(mono_class_property&& o) -> mono_class_property& = default;

auto mono_class_property::get_internal_ptr() const -> MonoProperty*
{
	return property_;
}

} // namespace mono
