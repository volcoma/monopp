#pragma once

#include "mono_config.h"
#include "mono_type_traits.h"

#include "mono_class_field.h"
#include "mono_class_property.h"
#include "mono_method_thunk.h"
#include "mono_noncopyable.h"

#include <mono/metadata/class.h>
#include <mono/metadata/image.h>

namespace mono
{

class mono_assembly;

class mono_class : public common::noncopyable
{
public:
	explicit mono_class(const mono_assembly* assembly, MonoClass* cls);
	explicit mono_class(const mono_assembly* assembly, MonoImage* image, const std::string& name);
	explicit mono_class(const mono_assembly* assembly, MonoImage* image, const std::string& name_space,
						const std::string& name);

	mono_class(mono_class&& o);
	auto operator=(mono_class&& o) -> mono_class&;

	auto get_static_method(const std::string& name_with_args) const -> mono_method;
	auto get_static_method(const std::string& name, int argc) const -> mono_method;

	template <typename function_signature_t>
	auto get_static_method(const std::string& name) const;

	auto get_field(const std::string& name) const -> mono_class_field;
	auto get_property(const std::string& name) const -> mono_class_property;

	auto get_name() const -> std::string;

	auto get_fields() const -> std::vector<mono_class_field>;
	auto get_properties() const -> std::vector<mono_class_property>;

	template <typename T>
	auto get_static_field_value(const mono_class_field& field) const -> T;

	template <typename T>
	void set_static_field_value(const mono_class_field& field, const T& val) const;

	template <typename T>
	auto get_static_property_value(const mono_class_property& prop) const -> T;

	template <typename T>
	void set_static_property_value(const mono_class_property& prop, const T& val) const;

	auto is_valuetype() const -> bool;

	auto get_internal_ptr() const -> MonoClass*;

private:
	const mono_assembly* assembly_ = nullptr;
	MonoClass* class_ = nullptr;
};

template <typename function_signature_t>
auto mono_class::get_static_method(const std::string& name) const
{
	using arg_types = typename function_traits<function_signature_t>::arg_types;
	arg_types tup;
	auto args_result = types::get_args_signature(tup);
	auto args = args_result.first;
	auto all_types_known = args_result.second;

	if(all_types_known)
	{
		auto func = get_static_method(name + "(" + args + ")");
		return mono_method_thunk<function_signature_t>(std::move(func));
	}
	else
	{
		constexpr auto arg_count = function_traits<function_signature_t>::arity;
		auto func = get_static_method(name, arg_count);
		return mono_method_thunk<function_signature_t>(std::move(func));
	}
}

template <typename T>
auto mono_class::get_static_field_value(const mono_class_field& field) const -> T
{
	T val{};
	assert(field.get_internal_ptr());
	MonoObject* refvalue = nullptr;
	auto arg = reinterpret_cast<void*>(&val);
	if(!field.is_valuetype())
	{
		arg = &refvalue;
	}
	auto domain = assembly_->get_domain()->get_internal_ptr();
	MonoVTable* vtable = mono_class_vtable(domain, class_);
	mono_runtime_class_init(vtable);

	mono_field_static_get_value(vtable, field.get_internal_ptr(), arg);

	if(!field.is_valuetype())
	{
		val = convert_mono_type<T>::from_mono(refvalue);
	}
	return val;
}

template <typename T>
void mono_class::set_static_field_value(const mono_class_field& field, const T& val) const
{
	assert(field.get_internal_ptr());

	auto mono_val = convert_mono_type<T>::to_mono(*assembly_, val);
	auto arg = to_mono_arg(mono_val);

	auto domain = assembly_->get_domain()->get_internal_ptr();
	MonoVTable* vtable = mono_class_vtable(domain, class_);
	mono_runtime_class_init(vtable);

	mono_field_static_set_value(vtable, field.get_internal_ptr(), arg);
}

template <typename T>
auto mono_class::get_static_property_value(const mono_class_property& prop) const -> T
{
	assert(prop.get_internal_ptr());
	MonoObject* ex = nullptr;
	auto obj = mono_property_get_value(prop.get_internal_ptr(), nullptr, nullptr, &ex);

	if(ex)
	{
		throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
	}

	T val = convert_mono_type<T>::from_mono(obj);

	return val;
}

template <typename T>
void mono_class::set_static_property_value(const mono_class_property& prop, const T& val) const
{
	assert(prop.get_internal_ptr());

	MonoObject* ex = nullptr;
	auto mono_val = convert_mono_type<T>::to_mono(*assembly_, val);
	void* argsv[] = {to_mono_arg(mono_val)};
	mono_property_set_value(prop.get_internal_ptr(), nullptr, argsv, &ex);

	if(ex)
	{
		throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
	}
}
} // namespace mono
