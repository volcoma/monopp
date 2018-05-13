#pragma once

#include "mono_config.h"
#include "mono_type_traits.h"

#include "mono_class_field.h"
#include "mono_class_property.h"
#include "mono_domain.h"
#include "mono_method_thunk.h"
#include "mono_object.h"
#include "mono_type_conversion.h"

namespace mono
{

class mono_domain;
class mono_class;
class mono_assembly;

class mono_class_instance : public mono_object
{
public:
	explicit mono_class_instance(MonoObject* obj);
	explicit mono_class_instance(mono_assembly* assembly, mono_domain* domain, MonoClass* cls);

	mono_class_instance(mono_class_instance&& o);
	auto operator=(mono_class_instance&& o) -> mono_class_instance&;

	auto get_method(const std::string& name, int argc = 0) -> mono_method;

	template <typename function_signature_t>
	auto get_method(const std::string& name);

	auto get_class() -> mono_class;

	template <typename T>
	auto get_field_value(const mono_class_field& field) const -> T;

	template <typename T>
	void get_field_value(const mono_class_field& field, T& val) const;

	template <typename T>
	void set_field_value(const mono_class_field& field, const T& val) const;

	template <typename T>
	auto get_property_value(const mono_class_property& prop) const -> T;

	template <typename T>
	void get_property_value(const mono_class_property& prop, T& val) const;

	template <typename T>
	void set_property_value(const mono_class_property& prop, const T& val) const;

private:
	MonoClass* class_ = nullptr;
	mono_assembly* assembly_ = nullptr;
};

template <typename function_signature_t>
auto mono_class_instance::get_method(const std::string& name)
{
	constexpr auto arg_count = function_traits<function_signature_t>::arity;
	auto func = get_method(name, arg_count);
	return mono_method_thunk<function_signature_t>(std::move(func));
}

template <typename T>
auto mono_class_instance::get_field_value(const mono_class_field& field) const -> T
{
	T val{};
	get_field_value(field, val);
	return val;
}

template <typename T>
void mono_class_instance::get_field_value(const mono_class_field& field, T& val) const
{
	assert(object_);
	assert(field.get_internal_ptr());
	MonoObject* refvalue = nullptr;
	auto arg = reinterpret_cast<void*>(&val);
	if(!field.is_valuetype())
	{
		arg = &refvalue;
	}

	mono_field_get_value(object_, field.get_internal_ptr(), arg);

	if(!field.is_valuetype())
	{
		val = convert_mono_type<T>::from_mono(refvalue);
	}
}

template <typename T>
void mono_class_instance::set_field_value(const mono_class_field& field, const T& val) const
{
	assert(object_);
	assert(field.get_internal_ptr());

	auto mono_val = convert_mono_type<T>::to_mono(*assembly_, val);
	auto arg = to_mono_arg(mono_val);

	mono_field_set_value(object_, field.get_internal_ptr(), arg);
}

template <typename T>
auto mono_class_instance::get_property_value(const mono_class_property& prop) const -> T
{
	T val{};
	get_property_value(prop, val);
	return val;
}

template <typename T>
void mono_class_instance::get_property_value(const mono_class_property& prop, T& val) const
{
	assert(object_);
	assert(prop.get_internal_ptr());
	MonoObject* ex = nullptr;
	auto obj = mono_property_get_value(prop.get_internal_ptr(), object_, nullptr, &ex);

	if(ex)
	{
		throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
	}

	val = convert_mono_type<T>::from_mono(obj);
}

template <typename T>
void mono_class_instance::set_property_value(const mono_class_property& prop, const T& val) const
{
	assert(object_);
	assert(prop.get_internal_ptr());

	MonoObject* ex = nullptr;
	auto mono_val = convert_mono_type<T>::to_mono(*assembly_, val);
	void* argsv[] = {to_mono_arg(mono_val)};
	mono_property_set_value(prop.get_internal_ptr(), object_, argsv, &ex);

	if(ex)
	{
		throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
	}
}

} // namespace mono
