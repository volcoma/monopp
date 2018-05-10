#include "mono_class_field.h"
#include "mono_class.h"
#include <cassert>

namespace mono
{

mono_class_field::mono_class_field(const mono_class& cls, const std::string& name)
	: field_(mono_class_get_field_from_name(cls.get_internal_ptr(), name.c_str()))
{
	if(!field_)
	{
		std::string cls_name = cls.get_name();
		throw mono_exception("NATIVE::Could not get field : " + name + " for class " + cls_name);
	}
	auto type = mono_field_get_type(field_);
	auto field_class = mono_class_from_mono_type(type);
	is_valuetype_ = !!mono_class_is_valuetype(field_class);
}

mono_class_field::mono_class_field(mono_class_field&& o) = default;

auto mono_class_field::operator=(mono_class_field&& o) -> mono_class_field& = default;

auto mono_class_field::get_internal_ptr() const -> MonoClassField*
{
	return field_;
}

auto mono_class_field::is_valuetype() const -> bool
{
	return is_valuetype_;
}

} // namespace mono
