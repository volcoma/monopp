#include "mono_property.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_method.h"

BEGIN_MONO_INCLUDE
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
} // namespace mono
