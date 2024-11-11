#include "mono_field.h"
#include "mono_domain.h"
#include "mono_exception.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/appdomain.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE
namespace mono
{

mono_field::mono_field(const mono_type& type, const std::string& name)
	: field_(mono_class_get_field_from_name(type.get_internal_ptr(), name.c_str()))
{
	if(!field_)
	{
		throw mono_exception("NATIVE::Could not get field : " + name + " for class " + type.get_name());
	}
	const auto& domain = mono_domain::get_current_domain();

	if(is_static())
	{
		owning_type_vtable_ = mono_class_vtable(domain.get_internal_ptr(), type.get_internal_ptr());
		//		mono_runtime_class_init(owning_type_vtable_);
	}

	auto field_type = mono_field_get_type(field_);
	type_ = mono_type(mono_class_from_mono_type(field_type));

	generate_meta();
}

void mono_field::generate_meta()
{
#ifndef NDEBUG
	meta_ = std::make_shared<meta_info>();

	meta_->name = get_name();
	meta_->fullname = get_fullname();
	meta_->full_declname = get_full_declname();

#endif
}

auto mono_field::is_valuetype() const -> bool
{
	return get_type().is_valuetype();
}

auto mono_field::get_name() const -> std::string
{
	return mono_field_get_name(field_);
}
auto mono_field::get_fullname() const -> std::string
{
	return mono_field_full_name(field_);
}

auto mono_field::get_full_declname() const -> std::string
{
	std::string storage = (is_static() ? " static " : " ");
	return to_string(get_visibility()) + storage + get_fullname();
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

auto mono_field::get_attributes() const -> std::vector<mono_type>
{
	std::vector<mono_type> result;

	auto parent_class = mono_field_get_parent(field_);
	// Get custom attributes from the field
	MonoCustomAttrInfo* attr_info = mono_custom_attrs_from_field(parent_class, field_);

	if(attr_info)
	{
		result.reserve(attr_info->num_attrs);

		// Iterate over the custom attributes
		for(int i = 0; i < attr_info->num_attrs; ++i)
		{
			MonoCustomAttrEntry* entry = &attr_info->attrs[i];

			// Get the MonoClass* of the attribute
			MonoClass* attr_class = mono_method_get_class(entry->ctor);

			// Add the attribute class to the result vector
			result.emplace_back(attr_class);
		}

		// Free the attribute info when done
		mono_custom_attrs_free(attr_info);
	}

	// Get field flags
	uint32_t flags = mono_field_get_flags(field_);

	MonoImage* corlib = mono_get_corlib(); // Get corlib once for efficiency

	// Check for NotSerialized (NonSerialized in .NET)
	if((flags & MONO_FIELD_ATTR_NOT_SERIALIZED) != 0)
	{
		MonoClass* non_serialized_attr_class =
			mono_class_from_name(corlib, "System", "NonSerializedAttribute");
		if(non_serialized_attr_class)
		{
			result.emplace_back(non_serialized_attr_class);
		}
	}

	// Check for SpecialName
	if((flags & MONO_FIELD_ATTR_SPECIAL_NAME) != 0)
	{
		MonoClass* special_name_attr_class =
			mono_class_from_name(corlib, "System.Runtime.CompilerServices", "SpecialNameAttribute");
		if(special_name_attr_class)
		{
			result.emplace_back(special_name_attr_class);
		}
	}

	// Check for RTSpecialName
	if((flags & MONO_FIELD_ATTR_RT_SPECIAL_NAME) != 0)
	{
		MonoClass* rt_special_name_attr_class =
			mono_class_from_name(corlib, "System.Runtime.CompilerServices", "RuntimeSpecialNameAttribute");
		if(rt_special_name_attr_class)
		{
			result.emplace_back(rt_special_name_attr_class);
		}
	}

	// Check for PinvokeImpl
	if((flags & MONO_FIELD_ATTR_PINVOKE_IMPL) != 0)
	{
		MonoClass* field_offset_attr_class =
			mono_class_from_name(corlib, "System.Runtime.InteropServices", "FieldOffsetAttribute");
		if(field_offset_attr_class)
		{
			result.emplace_back(field_offset_attr_class);
		}
	}

	// Check for Literal (const fields)
	if((flags & MONO_FIELD_ATTR_LITERAL) != 0)
	{
		// No direct attribute in .NET, but you can note this flag
		// Alternatively, you can represent it as a constant field
	}

	// Check for InitOnly (readonly fields)
	if((flags & MONO_FIELD_ATTR_INIT_ONLY) != 0)
	{
		// No direct attribute, but you can note this flag or create a method is_readonly()
	}

	return result;
}

auto mono_field::is_readonly() const -> bool
{
	uint32_t flags = mono_field_get_flags(field_);
	return (flags & MONO_FIELD_ATTR_INIT_ONLY) != 0;
}

auto mono_field::is_const() const -> bool
{
	uint32_t flags = mono_field_get_flags(field_);
	return (flags & MONO_FIELD_ATTR_LITERAL) != 0;
}
} // namespace mono
