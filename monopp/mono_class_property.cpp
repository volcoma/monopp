#include "mono_class_property.h"
#include "mono_assembly.h"
#include "mono_class.h"
#include "mono_exception.h"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>

namespace mono
{

mono_class_property::mono_class_property(const mono_class& cls, const std::string& name)
	: class_(cls)
	, property_(mono_class_get_property_from_name(cls.get_internal_ptr(), name.c_str()))
	, name_(name)
{
	if(!property_)
		throw mono_exception("NATIVE::Could not get property : " + name + " for class " + cls.get_name());

	auto get_method = get_get_method();
	type_ = get_method.get_return_type();
	fullname_ = name_;
}

auto mono_class_property::get_internal_ptr() const -> MonoProperty*
{
	return property_;
}

auto mono_class_property::get_name() const -> const std::string&
{
	return name_;
}

auto mono_class_property::get_fullname() const -> const std::string&
{
	return fullname_;
}

auto mono_class_property::get_owning_class() const -> const mono_class&
{
	return class_;
}
auto mono_class_property::get_type() const -> const mono_type&
{
    return type_;
}

auto mono_class_property::get_get_method() const -> mono_method
{
    auto method = mono_property_get_get_method(property_);
    return mono_method(class_, method);
}
auto mono_class_property::get_set_method() const -> mono_method
{
    auto method = mono_property_get_set_method(property_);
    return mono_method(class_, method);
}
} // namespace mono
