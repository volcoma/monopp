#include "mono_type.h"
#include "mono_assembly.h"
#include "mono_exception.h"

#include "mono_domain.h"
#include "mono_field.h"
#include "mono_method.h"
#include "mono_object.h"
#include "mono_property.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/appdomain.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE
#include <iostream>
namespace mono
{

mono_type::mono_type() = default;

mono_type::mono_type(MonoImage* image, const std::string& name)
	: mono_type(image, "", name)
{
}

mono_type::mono_type(MonoImage* image, const std::string& name_space, const std::string& name)
{
	class_ = mono_class_from_name(image, name_space.c_str(), name.c_str());

	if(!class_)
		throw mono_exception("NATIVE::Could not get class : " + name_space + "." + name);

	generate_meta();
}

mono_type::mono_type(MonoClass* cls)
{
	class_ = cls;
	if(!class_)
		throw mono_exception("NATIVE::Could not get class");

	generate_meta();
}
mono_type::mono_type(MonoType* type)
{
	class_ = mono_class_from_mono_type(type);
	if(!class_)
		throw mono_exception("NATIVE::Could not get class");

	generate_meta();
}
auto mono_type::valid() const -> bool
{
	return class_ != nullptr;
}

auto mono_type::new_instance() const -> mono_object
{
	const auto& domain = mono_domain::get_current_domain();
	return mono_object(domain, *this);
}

auto mono_type::get_method(const std::string& name_with_args) const -> mono_method
{
	return mono_method(*this, name_with_args);
}

auto mono_type::get_method(const std::string& name, int argc) const -> mono_method
{
	return mono_method(*this, name, argc);
}

auto mono_type::get_field(const std::string& name) const -> mono_field
{
	return mono_field(*this, name);
}

auto mono_type::get_property(const std::string& name) const -> mono_property
{
	return mono_property(*this, name);
}

auto mono_type::get_fields() const -> std::vector<mono_field>
{
	void* iter = nullptr;
	auto field = mono_class_get_fields(class_, &iter);
	std::vector<mono_field> fields;
	while(field)
	{
		std::string name = mono_field_get_name(field);

		fields.emplace_back(get_field(name));

		field = mono_class_get_fields(class_, &iter);
	}
	return fields;
}

auto mono_type::get_properties() const -> std::vector<mono_property>
{
	void* iter = nullptr;
	auto prop = mono_class_get_properties(class_, &iter);
	std::vector<mono_property> props;
	while(prop)
	{
		std::string name = mono_property_get_name(prop);

		props.emplace_back(get_property(name));

		prop = mono_class_get_properties(class_, &iter);
	}
	return props;
}

auto mono_type::get_methods() const -> std::vector<mono_method>
{
	void* iter = nullptr;
	auto method = mono_class_get_methods(class_, &iter);
	std::vector<mono_method> methods;

	while(method != nullptr)
	{
		auto sig = mono_method_signature(method);
		std::string signature = mono_signature_get_desc(sig, false);
		std::string name = mono_method_get_name(method);
		std::string fullname = name + "(" + signature + ")";
		methods.emplace_back(get_method(fullname));
		method = mono_class_get_methods(class_, &iter);
	}

	return methods;
}

auto mono_type::get_attributes() const -> std::vector<mono_type>
{
	std::vector<mono_type> result;

	// Get custom attributes from the class
	MonoCustomAttrInfo* attr_info = mono_custom_attrs_from_class(class_);

	if(attr_info)
	{
		result.reserve(attr_info->num_attrs);
		// Iterate over the custom attributes
		for(int i = 0; i < attr_info->num_attrs; ++i)
		{
			MonoCustomAttrEntry* entry = &attr_info->attrs[i];
			MonoClass* attr_class = mono_method_get_class(entry->ctor);
			result.emplace_back(attr_class);
		}

		// Free the attribute info when done
		mono_custom_attrs_free(attr_info);
	}

	// Get class flags
	uint32_t flags = mono_class_get_flags(class_);

	MonoImage* corlib = mono_get_corlib(); // Get corlib once for efficiency

	// Check for Serializable
	if((flags & MONO_TYPE_ATTR_SERIALIZABLE) != 0)
	{
		MonoClass* serializable_attr_class = mono_class_from_name(corlib, "System", "SerializableAttribute");
		if(serializable_attr_class)
		{
			result.emplace_back(serializable_attr_class);
		}
	}

	// Check for ComImport
	if((flags & MONO_TYPE_ATTR_IMPORT) != 0)
	{
		MonoClass* com_import_attr_class =
			mono_class_from_name(corlib, "System.Runtime.InteropServices", "ComImportAttribute");
		if(com_import_attr_class)
		{
			result.emplace_back(com_import_attr_class);
		}
	}

	// Check for SpecialName
	if((flags & MONO_TYPE_ATTR_SPECIAL_NAME) != 0)
	{
		MonoClass* special_name_attr_class =
			mono_class_from_name(corlib, "System.Runtime.CompilerServices", "SpecialNameAttribute");
		if(special_name_attr_class)
		{
			result.emplace_back(special_name_attr_class);
		}
	}

	// Check for StructLayout
	uint32_t layout = flags & MONO_TYPE_ATTR_LAYOUT_MASK;
	if(layout == MONO_TYPE_ATTR_SEQUENTIAL_LAYOUT || layout == MONO_TYPE_ATTR_EXPLICIT_LAYOUT)
	{
		MonoClass* struct_layout_attr_class =
			mono_class_from_name(corlib, "System.Runtime.InteropServices", "StructLayoutAttribute");
		if(struct_layout_attr_class)
		{
			result.emplace_back(struct_layout_attr_class);
		}
	}

	// Check for Abstract
	if((flags & MONO_TYPE_ATTR_ABSTRACT) != 0)
	{
		// There is no AbstractAttribute, but you can note this flag
		// Alternatively, you can create a custom representation
	}

	// Check for Sealed
	if((flags & MONO_TYPE_ATTR_SEALED) != 0)
	{
		// There is no SealedAttribute, but you can note this flag
	}

	// Check for Interface
	if((flags & MONO_TYPE_ATTR_INTERFACE) != 0)
	{
		// There is no InterfaceAttribute, but you can note this flag
	}

	// Check for BeforeFieldInit
	if((flags & MONO_TYPE_ATTR_BEFORE_FIELD_INIT) != 0)
	{
		// There is no BeforeFieldInitAttribute, but you can note this flag
	}

	// Check for HasSecurity
	if((flags & MONO_TYPE_ATTR_HAS_SECURITY) != 0)
	{
		// There is no HasSecurityAttribute, but you can note this flag
	}

	// Check for UnicodeClass (string format)
	uint32_t string_format = flags & MONO_TYPE_ATTR_STRING_FORMAT_MASK;
	if(string_format == MONO_TYPE_ATTR_UNICODE_CLASS)
	{
		// MonoClass* unicode_class_attr = mono_class_from_name(corlib, "System.Runtime.CompilerServices",
		// "UnicodeClassAttribute");
		//  Note: There is no UnicodeClassAttribute in .NET
		//  You can decide how to represent this flag
	}

	// Check for AutoClass (string format)
	if(string_format == MONO_TYPE_ATTR_AUTO_CLASS)
	{
		// Similar to above
	}

	// Additional pseudo-custom attributes can be added similarly

	return result;
}

auto mono_type::has_base_type() const -> bool
{
	return mono_class_get_parent(class_) != nullptr;
}

auto mono_type::get_base_type() const -> mono_type
{
	auto base = mono_class_get_parent(class_);
	return mono_type(base);
}

auto mono_type::get_nested_types() const -> std::vector<mono_type>
{
	void* iter = nullptr;
	auto nested = mono_class_get_nested_types(class_, &iter);
	std::vector<mono_type> nested_clases;
	while(nested)
	{
		nested_clases.emplace_back(mono_type(nested));
	}
	return nested_clases;
}

auto mono_type::get_internal_ptr() const -> MonoClass*
{
	return class_;
}

void mono_type::generate_meta()
{
	type_ = mono_class_get_type(class_);
	namespace_ = mono_class_get_namespace(class_);
	name_ = mono_class_get_name(class_);
	fullname_ = namespace_.empty() ? name_ : namespace_ + "." + name_;
	rank_ = mono_class_get_rank(class_);
	valuetype_ = !!mono_class_is_valuetype(class_);
	enum_ = mono_class_is_enum(class_);
	sizeof_ = std::uint32_t(mono_class_value_size(class_, &alignof_));
}

auto mono_type::is_derived_from(const mono_type& type) const -> bool
{
	return mono_class_is_subclass_of(class_, type.get_internal_ptr(), true) != 0;
}
auto mono_type::get_namespace() const -> const std::string&
{
	return namespace_;
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

auto mono::mono_type::mono_type::is_enum() const -> bool
{
	return enum_;
}

auto mono::mono_type::mono_type::is_class() const -> bool
{
	return !is_valuetype();
}

auto mono::mono_type::mono_type::is_struct() const -> bool
{
	return is_valuetype() && !is_enum();
}

auto mono_type::get_rank() const -> int
{
	return rank_;
}

auto mono_type::get_sizeof() const -> uint32_t
{
	return sizeof_;
}

auto mono_type::get_alignof() const -> uint32_t
{
	return alignof_;
}

auto mono_type::is_abstract() const -> bool
{
	return (mono_class_get_flags(class_) & MONO_TYPE_ATTR_ABSTRACT) != 0;
}

auto mono_type::is_sealed() const -> bool
{
	return (mono_class_get_flags(class_) & MONO_TYPE_ATTR_SEALED) != 0;
}

auto mono_type::is_interface() const -> bool
{
	return (mono_class_get_flags(class_) & MONO_TYPE_ATTR_INTERFACE) != 0;
}
} // namespace mono
