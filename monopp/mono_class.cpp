#include "mono_class.h"
#include "mono_assembly.h"
#include "mono_exception.h"

#include "mono_class_field.h"
#include "mono_class_instance.h"
#include "mono_class_property.h"
#include "mono_method.h"

#include <mono/metadata/debug-helpers.h>
namespace mono
{

mono_class::mono_class(MonoImage* image, const std::string& name)
	: mono_class(image, "", name)
{
}

mono_class::mono_class(MonoImage* image, const std::string& name_space, const std::string& name)
{
	class_ = mono_class_from_name(image, name_space.c_str(), name.c_str());

	if(!class_)
		throw mono_exception("NATIVE::Could not get class : " + name_space + "." + name);

	__generate_meta();
}

mono_class::mono_class(MonoClass* cls)
{
	class_ = cls;
	if(!class_)
		throw mono_exception("NATIVE::Could not get class");

	__generate_meta();
}

auto mono_class::operator=(MonoClass* cls) -> mono_class&
{
	class_ = cls;
	if(!class_)
		throw mono_exception("NATIVE::Could not get class");

	__generate_meta();

	return *this;
}

auto mono_class::valid() const -> bool
{
	return class_ != nullptr;
}

auto mono_class::new_instance() const -> mono_class_instance
{
	const auto& domain = mono_domain::get_current_domain();
	return mono_class_instance(domain, *this);
}

auto mono_class::get_method(const std::string& name_with_args) const -> mono_method
{
	return mono_method(*this, name_with_args);
}

auto mono_class::get_method(const std::string& name, int argc) const -> mono_method
{
	return mono_method(*this, name, argc);
}

auto mono_class::get_field(const std::string& name) const -> mono_class_field
{
	return mono_class_field(*this, name);
}

auto mono_class::get_property(const std::string& name) const -> mono_class_property
{
	return mono_class_property(*this, name);
}

auto mono_class::get_fields() const -> std::vector<mono_class_field>
{
	void* iter = nullptr;
	auto field = mono_class_get_fields(class_, &iter);
	std::vector<mono_class_field> fields;
	while(field)
	{
		std::string name = mono_field_get_name(field);

		fields.emplace_back(get_field(name));

		field = mono_class_get_fields(class_, &iter);
	}
	return fields;
}

auto mono_class::get_properties() const -> std::vector<mono_class_property>
{
	void* iter = nullptr;
	auto prop = mono_class_get_properties(class_, &iter);
	std::vector<mono_class_property> props;
	while(prop)
	{
		std::string name = mono_property_get_name(prop);

		props.emplace_back(get_property(name));

		prop = mono_class_get_properties(class_, &iter);
	}
	return props;
}

auto mono_class::get_methods() const -> std::vector<mono_method>
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

auto mono_class::has_base_class() const -> bool
{
	return mono_class_get_parent(class_) != nullptr;
}

auto mono_class::get_base_class() const -> mono_class
{
	auto base = mono_class_get_parent(class_);
	return mono_class(base);
}

auto mono_class::get_nested_classes() const -> std::vector<mono_class>
{
	void* iter = nullptr;
	auto nested = mono_class_get_nested_types(class_, &iter);
	std::vector<mono_class> nested_clases;
	while(nested)
	{
		nested_clases.emplace_back(mono_class(nested));
	}
	return nested_clases;
}

auto mono_class::get_internal_ptr() const -> MonoClass*
{
	return class_;
}

void mono_class::__generate_meta()
{
	namespace_ = mono_class_get_namespace(class_);
	name_ = mono_class_get_name(class_);
	fullname_ = namespace_.empty() ? name_ : namespace_ + "." + name_;
	rank_ = mono_class_get_rank(class_);
	valuetype_ = !!mono_class_is_valuetype(class_);
	sizeof_ = std::uint32_t(mono_class_value_size(class_, &alignof_));
}

auto mono_class::is_subclass_of(const mono_class& cls) const -> bool
{
	return mono_class_is_subclass_of(class_, cls.get_internal_ptr(), false) != 0;
}
auto mono_class::get_namespace() const -> const std::string&
{
	return namespace_;
}
auto mono_class::get_name() const -> const std::string&
{
	return name_;
}
auto mono_class::get_fullname() const -> const std::string&
{
	return fullname_;
}
auto mono_class::is_valuetype() const -> bool
{
	return valuetype_;
}

auto mono_class::get_rank() const -> int
{
	return rank_;
}

uint32_t mono_class::get_sizeof() const
{
	return sizeof_;
}

uint32_t mono_class::get_alignof() const
{
	return alignof_;
}
} // namespace mono
