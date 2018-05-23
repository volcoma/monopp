#include "mono_class.h"
#include "mono_class_field.h"
#include "mono_class_property.h"
#include "mono_exception.h"
#include "mono_method.h"

namespace mono
{

mono_class::mono_class(const mono_assembly* assembly, MonoClass* cls)
	: assembly_(assembly)
	, class_(cls)
{
}

mono_class::mono_class(const mono_assembly* assembly, MonoImage* image, const std::string& name)
	: mono_class(assembly, image, "", name)
{
}

mono_class::mono_class(const mono_assembly* assembly, MonoImage* image, const std::string& name_space,
					   const std::string& name)
	: assembly_(assembly)
{
	class_ = mono_class_from_name(image, name_space.c_str(), name.c_str());

	if(!class_)
		throw mono_exception("NATIVE::Could not get class : " + name_space + "::" + name);
}

mono_class::mono_class(mono_class&&) = default;

auto mono_class::operator=(mono_class &&) -> mono_class& = default;

auto mono_class::get_static_method(const std::string& name_with_args) const -> mono_method
{
	assert(class_);
	assert(assembly_);
	return mono_method(assembly_, class_, nullptr, name_with_args);
}

auto mono_class::get_static_method(const std::string& name, int argc) const -> mono_method
{
	assert(class_);
	assert(assembly_);
	return mono_method(assembly_, class_, nullptr, name, argc);
}

auto mono_class::get_internal_ptr() const -> MonoClass*
{
	assert(class_);
	return class_;
}

auto mono_class::get_field(const std::string& name) const -> mono_class_field
{
	return mono_class_field(*this, name);
}

auto mono_class::get_property(const std::string& name) const -> mono_class_property
{
	return mono_class_property(*this, name);
}

auto mono_class::get_name() const -> std::string
{
	return mono_class_get_name(get_internal_ptr());
}

auto mono_class::get_fields() const -> std::vector<mono_class_field>
{
	void* iter = nullptr;
	MonoClassField* field = mono_class_get_fields(class_, &iter);
	std::vector<mono_class_field> fields;
	while(field)
	{
		std::string name = mono_field_get_name(field);
		fields.emplace_back(*this, name);

		field = mono_class_get_fields(class_, &iter);
	}
	return fields;
}

auto mono_class::get_properties() const -> std::vector<mono_class_property>
{
	void* iter = nullptr;
	MonoProperty* prop = mono_class_get_properties(class_, &iter);
	std::vector<mono_class_property> props;
	while(prop)
	{
		std::string name = mono_property_get_name(prop);
		props.emplace_back(*this, name);

		prop = mono_class_get_properties(class_, &iter);
	}
	return props;
}

auto mono_class::is_valuetype() const -> bool
{
    return !!mono_class_is_valuetype(get_internal_ptr());
}

} // namespace mono
