#include "mono_property.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_method.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/appdomain.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE

namespace mono
{

mono_property::mono_property(const mono_type& type, const std::string& name)
	: property_(mono_class_get_property_from_name(type.get_internal_ptr(), name.c_str()))
	, name_(name)
{
	if(!property_)
		throw mono_exception("NATIVE::Could not get property : " + name + " for class " + type.get_name());

	generate_meta();
}

auto mono_property::get_internal_ptr() const -> MonoProperty*
{
	return property_;
}

auto mono_property::get_name() const -> const std::string&
{
	return name_;
}

auto mono_property::get_fullname() const -> const std::string&
{
	return fullname_;
}
auto mono_property::get_full_declname() const -> const std::string&
{
	return full_declname_;
}

auto mono_property::get_type() const -> const mono_type&
{
	return type_;
}

auto mono_property::get_get_method() const -> mono_method
{
	auto method = mono_property_get_get_method(property_);
	return mono_method(method);
}
auto mono_property::get_set_method() const -> mono_method
{
	auto method = mono_property_get_set_method(property_);
	return mono_method(method);
}

auto mono_property::get_visibility() const -> visibility
{
	auto getter_vis = visibility::vis_public;
	try
	{
		auto getter = get_get_method();
		getter_vis = getter.get_visibility();
	}
	catch(const mono_exception&)
	{
	}
	auto setter_vis = visibility::vis_public;
	try
	{
		auto setter = get_set_method();
		setter_vis = setter.get_visibility();
	}
	catch(const mono_exception&)
	{
	}
	if(int(getter_vis) < int(setter_vis))
	{
		return getter_vis;
	}

	return setter_vis;
}

auto mono_property::is_static() const -> bool
{
	auto getter = get_get_method();
	return getter.is_static();
}

void mono_property::generate_meta()
{
	auto get_method = get_get_method();
	type_ = mono_type(get_method.get_return_type());
	fullname_ = name_;
	std::string storage = (is_static() ? " static " : " ");
	full_declname_ = to_string(get_visibility()) + storage + fullname_;
}

auto mono_property::get_attributes() const -> std::vector<mono_type>
{
	std::vector<mono_type> result;

	// Get custom attributes from the property
	MonoCustomAttrInfo* attr_info = mono_custom_attrs_from_property(type_.get_internal_ptr(), property_);

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

	// Get property flags
	uint32_t flags = mono_property_get_flags(property_);

	MonoImage* corlib = mono_get_corlib(); // Get corlib once for efficiency

	// Check for SpecialName
	if((flags & MONO_PROPERTY_ATTR_SPECIAL_NAME) != 0)
	{
		MonoClass* special_name_attr_class =
			mono_class_from_name(corlib, "System.Runtime.CompilerServices", "SpecialNameAttribute");
		if(special_name_attr_class)
		{
			result.emplace_back(special_name_attr_class);
		}
	}

	// Check for RTSpecialName
	if((flags & MONO_PROPERTY_ATTR_RT_SPECIAL_NAME) != 0)
	{
		MonoClass* rt_special_name_attr_class =
			mono_class_from_name(corlib, "System.Runtime.CompilerServices", "RuntimeSpecialNameAttribute");
		if(rt_special_name_attr_class)
		{
			result.emplace_back(rt_special_name_attr_class);
		}
	}

	// Check for HasDefault (if needed)
	if((flags & MONO_PROPERTY_ATTR_HAS_DEFAULT) != 0)
	{
		MonoClass* default_member_attr_class =
			mono_class_from_name(corlib, "System.Reflection", "DefaultMemberAttribute");
		if(default_member_attr_class)
		{
			result.emplace_back(default_member_attr_class);
		}
	}

	return result;
}

auto mono_property::is_special_name() const -> bool
{
	uint32_t flags = mono_property_get_flags(property_);
	return (flags & MONO_PROPERTY_ATTR_SPECIAL_NAME) != 0;
}

auto mono_property::has_default() const -> bool
{
	uint32_t flags = mono_property_get_flags(property_);
	return (flags & MONO_PROPERTY_ATTR_HAS_DEFAULT) != 0;
}
} // namespace mono
