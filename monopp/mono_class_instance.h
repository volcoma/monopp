#pragma once

#include "mono_config.h"
#include "mono_type_traits.h"

#include "mono_object.h"
#include "mono_class_field.h"
#include "mono_class_property.h"

#include <cassert>
#include <string>

namespace mono
{

class mono_domain;
class mono_method;
class mono_class;
class mono_assembly;

class mono_class_instance : public mono_object
{
public:
	mono_class_instance() = default;
	explicit mono_class_instance(MonoObject* obj);
	explicit mono_class_instance(mono_assembly* assembly, MonoObject* obj);
	explicit mono_class_instance(mono_assembly* assembly, mono_domain* domain, MonoClass* cls);

	mono_class_instance(mono_class_instance&& o);
	auto operator=(mono_class_instance&& o) -> mono_class_instance&;

	auto get_method(const std::string& name, int argc = 0) -> mono_method;

	template <typename function_signature_t>
	auto get_method_thunk(const std::string& name);

	auto get_class() -> mono_class;

	auto get_mono_class_ptr() -> MonoClass*;

	template <typename T>
	auto get_field_value(mono_class_field& field) const;

	template <typename T>
	void get_field_value(mono_class_field& field, T& val) const;

	template <typename T>
	void set_field_value(mono_class_field& field, const T& val) const;
    
    template <typename T>
	auto get_property_value(mono_class_property& prop) const;

	template <typename T>
	void get_property_value(mono_class_property& prop, T& val) const;

	template <typename T>
	void set_property_value(mono_class_property& prop, const T& val) const;

private:
	MonoClass* class_ = nullptr;
	mono_assembly* assembly_ = nullptr;
};

template <typename function_signature_t>
auto mono_class_instance::get_method_thunk(const std::string& name)
{
	constexpr auto arg_count = function_traits<function_signature_t>::arity;
	auto func = get_method(name, arg_count);
	return func.template get_thunk<function_signature_t>();
}

template <typename T>
auto mono_class_instance::get_field_value(mono_class_field& field) const
{
    T val{};
	get_field_value(field, val);
	return val;
}

template <typename T>
void mono_class_instance::get_field_value(mono_class_field& field, T& val) const
{
	assert(object_);
	assert(field.get_mono_class_field_ptr());
	mono_field_get_value(object_, field.get_mono_class_field_ptr(), reinterpret_cast<void*>(&val));
}

template <typename T>
void mono_class_instance::set_field_value(mono_class_field& field, const T& val) const
{
	assert(object_);
	assert(field.get_mono_class_field_ptr());
	mono_field_set_value(object_, field.get_mono_class_field_ptr(), const_cast<void*>(reinterpret_cast<const void*>(&val)));
}

template <typename T>
auto mono_class_instance::get_property_value(mono_class_property& prop) const
{
    T val{};
	get_property_value(prop, val);
	return val;
}

template <typename T>
void mono_class_instance::get_property_value(mono_class_property& prop, T& val) const
{
	assert(object_);
	assert(prop.get_mono_class_property_ptr());
    void* args[1];
    args[0] = reinterpret_cast<void*>(&val);
	mono_property_get_value(prop.get_mono_class_property_ptr(), object_, args, nullptr);
}

template <typename T>
void mono_class_instance::set_property_value(mono_class_property& prop, const T& val) const
{
	assert(object_);
	assert(prop.get_mono_class_property_ptr());
	//mono_property_set_value(prop.get_mono_class_property_ptr(), object_, (void**)&val), nullptr);
}

} // namespace mono
