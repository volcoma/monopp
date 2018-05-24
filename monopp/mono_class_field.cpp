#include "mono_class_field.h"
#include "mono_class.h"
#include "mono_exception.h"

namespace mono
{

mono_class_field::mono_class_field(const mono_class& cls, const std::string& name)
	: class_(cls)
	, field_(mono_class_get_field_from_name(cls.get_internal_ptr(), name.c_str()))
	, name_(name)
{
	if(!field_)
	{
		std::string cls_name = cls.get_name();
		throw mono_exception("NATIVE::Could not get field : " + name + " for class " + cls_name);
	}
	auto field_type = mono_field_get_type(field_);
	auto field_class = mono_class_from_mono_type(field_type);
	valuetype_ = !!mono_class_is_valuetype(field_class);
}

auto mono_class_field::get_internal_ptr() const -> MonoClassField*
{
	return field_;
}

auto mono_class_field::get_class() const -> const mono_class&
{
	return class_;
}

auto mono_class_field::is_valuetype() const -> bool
{
	return valuetype_;
}

auto mono_class_field::get_name() const -> std::string
{
	return name_;
}

} // namespace mono
