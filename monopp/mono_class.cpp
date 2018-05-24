#include "mono_class.h"
#include "mono_assembly.h"
#include "mono_exception.h"

#include "mono_class_field.h"
#include "mono_class_property.h"
#include "mono_method.h"

namespace mono
{

mono_class::mono_class(const mono_assembly& assembly, const std::string& name)
	: mono_class(assembly, "", name)
{
}

mono_class::mono_class(const mono_assembly& assembly, const std::string& name_space, const std::string& name)
	: assembly_(assembly)
	, namespace_(name_space)
	, name_(name)
{
	auto image_ptr = assembly.get_image_ptr();
	class_ = mono_class_from_name(image_ptr, name_space.c_str(), name.c_str());

	if(!class_)
		throw mono_exception("NATIVE::Could not get class : " + name_space + "." + name);

	valuetype_ = !!mono_class_is_valuetype(get_internal_ptr());
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

auto mono_class::get_internal_ptr() const -> MonoClass*
{
	assert(class_);
	return class_;
}

auto mono_class::get_assembly() const -> const mono_assembly&
{
	return assembly_;
}

auto mono_class::get_name() const -> const std::string&
{
	return name_;
}

auto mono_class::is_valuetype() const -> bool
{
	return valuetype_;
}

} // namespace mono
