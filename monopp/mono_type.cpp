#include "mono_type.h"
#include "mono_assembly.h"
#include "mono_exception.h"

#include "mono_domain.h"
#include "mono_field.h"
#include "mono_method.h"
#include "mono_object.h"
#include "mono_property.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE

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
	sizeof_ = std::uint32_t(mono_class_value_size(class_, &alignof_));
}

auto mono_type::is_derived_from(const mono_type& type) const -> bool
{
	return mono_class_is_subclass_of(class_, type.get_internal_ptr(), false) != 0;
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

auto mono_type::get_rank() const -> int
{
	return rank_;
}

uint32_t mono_type::get_sizeof() const
{
	return sizeof_;
}

uint32_t mono_type::get_alignof() const
{
	return alignof_;
}
} // namespace mono
