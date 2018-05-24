#include "mono_class_property.h"
#include "mono_assembly.h"
#include "mono_class.h"
#include "mono_exception.h"
namespace mono
{

mono_class_property::mono_class_property(const mono_class& cls, const std::string& name)
	: class_(cls)
	, property_(mono_class_get_property_from_name(cls.get_internal_ptr(), name.c_str()))
	, name_(name)
{
	if(!property_)
		throw mono_exception("NATIVE::Could not get property : " + name + " for class " + cls.get_name());

	auto method = mono_property_get_get_method(property_);
	auto sig = mono_method_get_signature(method, 0, 0);
	auto prop_type = mono_signature_get_return_type(sig);
	auto prop_class = mono_class_from_mono_type(prop_type);
	valuetype_ = !!mono_class_is_valuetype(prop_class);
}

auto mono_class_property::get_internal_ptr() const -> MonoProperty*
{
	return property_;
}

auto mono_class_property::get_name() const -> std::string
{
	return name_;
}

auto mono_class_property::is_valuetype() const -> bool
{
	return valuetype_;
}

auto mono_class_property::get_class() const -> const mono_class&
{
	return class_;
}

} // namespace mono
