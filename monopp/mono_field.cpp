#include "mono_field.h"
#include "mono_domain.h"
#include "mono_exception.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE
namespace mono
{

mono_field::mono_field(const mono_type& type, const std::string& name)
	: field_(mono_class_get_field_from_name(type.get_internal_ptr(), name.c_str()))
	, name_(name)
{
	if(!field_)
	{
		throw mono_exception("NATIVE::Could not get field : " + name + " for class " + type.get_name());
	}
	const auto& domain = mono_domain::get_current_domain();

	generate_meta();
	if(is_static())
	{
		owning_type_vtable_ = mono_class_vtable(domain.get_internal_ptr(), type.get_internal_ptr());
		mono_runtime_class_init(owning_type_vtable_);
	}
}

void mono_field::generate_meta()
{
	auto type = mono_field_get_type(field_);
	type_ = mono_type(mono_class_from_mono_type(type));
	fullname_ = mono_field_full_name(field_);
	std::string storage = (is_static() ? " static " : " ");
	full_declname_ = to_string(get_visibility()) + storage + fullname_;
}

auto mono_field::is_valuetype() const -> bool
{
	return get_type().is_valuetype();
}

auto mono_field::get_name() const -> const std::string&
{
	return name_;
}
auto mono_field::get_fullname() const -> const std::string&
{
	return fullname_;
}
auto mono_field::get_full_declname() const -> const std::string&
{
	return full_declname_;
}
auto mono_field::get_type() const -> const mono_type&
{
	return type_;
}

auto mono_field::get_visibility() const -> visibility
{
	uint32_t flags = mono_field_get_flags(field_) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

	if(flags == MONO_FIELD_ATTR_PRIVATE)
		return visibility::vis_private;
	else if(flags == MONO_FIELD_ATTR_FAM_AND_ASSEM)
		return visibility::vis_protected_internal;
	else if(flags == MONO_FIELD_ATTR_ASSEMBLY)
		return visibility::vis_internal;
	else if(flags == MONO_FIELD_ATTR_FAMILY)
		return visibility::vis_protected;
	else if(flags == MONO_FIELD_ATTR_PUBLIC)
		return visibility::vis_public;

	assert(false);

	return visibility::vis_private;
}

auto mono_field::is_static() const -> bool
{
	uint32_t flags = mono_field_get_flags(field_);

	return (flags & MONO_FIELD_ATTR_STATIC) != 0;
}
} // namespace mono
