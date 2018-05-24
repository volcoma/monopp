#include "mono_class_field.h"
#include "mono_class.h"
#include "mono_exception.h"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
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
	type_ = mono_field_get_type(field_);
	fullname_ = mono_field_full_name(field_);
}

auto mono_class_field::get_internal_ptr() const -> MonoClassField*
{
	return field_;
}

auto mono_class_field::get_owning_class() const -> const mono_class&
{
	return class_;
}

auto mono_class_field::get_name() const -> const std::string&
{
	return name_;
}
auto mono_class_field::get_fullname() const -> const std::string&
{
	return fullname_;
}

auto mono_class_field::get_type() const -> const mono_type&
{
	return type_;
}

auto mono_class_field::get_visibility() const -> visibility
{
	uint32_t flags = mono_field_get_flags(field_) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

	if(flags == MONO_FIELD_ATTR_PRIVATE)
		return visibility::private_;
	else if(flags == MONO_FIELD_ATTR_FAM_AND_ASSEM)
		return visibility::protected_internal_;
	else if(flags == MONO_FIELD_ATTR_ASSEMBLY)
		return visibility::internal_;
	else if(flags == MONO_FIELD_ATTR_FAMILY)
		return visibility::protected_;
	else if(flags == MONO_FIELD_ATTR_PUBLIC)
		return visibility::public_;

	assert(false);

	return visibility::private_;
}

auto mono_class_field::is_static() const -> bool
{
	uint32_t flags = mono_field_get_flags(field_);

	return (flags & MONO_FIELD_ATTR_STATIC) != 0;
}
} // namespace mono
