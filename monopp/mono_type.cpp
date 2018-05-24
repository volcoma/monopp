#include "mono_type.h"

#include <mono/metadata/debug-helpers.h>
namespace mono
{

mono_type::mono_type(MonoType* type)
	: type_(type)
{
	auto field_class = mono_class_from_mono_type(type_);
	valuetype_ = !!mono_class_is_valuetype(field_class);
	name_ = mono_type_get_name(type_);
	fullname_ = mono_type_full_name(type_);
}

auto mono_type::operator=(MonoType* type) -> mono_type&
{
	type_ = type;
	auto field_class = mono_class_from_mono_type(type_);
	valuetype_ = !!mono_class_is_valuetype(field_class);
	name_ = mono_type_get_name(type_);
	fullname_ = mono_type_full_name(type_);
	return *this;
}

auto mono_type::get_name() const -> const std::string&
{
	return name_;
}
auto mono_type::get_fullname() const -> const std::string&
{
	return fullname_;
}
auto mono_type::is_valuetype() const -> bool
{
	return valuetype_;
}

} // namespace mono
