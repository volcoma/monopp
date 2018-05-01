#include "mono_class_field.h"
#include "mono_class.h"
#include <cassert>

namespace mono
{

mono_class_field::mono_class_field(const mono_class& monoclass, const std::string& name)
	: field_(mono_class_get_field_from_name(monoclass.get_mono_class_ptr(), name.c_str()))
{
    if (!field_)
        throw mono_exception("Could not get field : " + name + " for class " + monoclass.get_name());
  
}

mono_class_field::mono_class_field(mono_class_field&& o) = default;

auto mono_class_field::operator=(mono_class_field&& o) -> mono_class_field& = default;

auto mono_class_field::get_mono_class_field_ptr() const -> MonoClassField*
{
	return field_;
}

} // namespace mono
