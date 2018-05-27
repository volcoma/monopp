#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_object.h"
#include "mono_type_conversion.h"
#include "mono_visibility.h"

namespace mono
{
class mono_type;

class mono_field
{
public:
	explicit mono_field(const mono_type& type, const std::string& name);

	template <typename T>
	void set_value(const T& val) const;

	template <typename T>
	void set_value(const mono_object& obj, const T& val) const;

	template <typename T>
	auto get_value() const -> T;

	template <typename T>
	auto get_value(const mono_object& obj) const -> T;

	auto get_name() const -> const std::string&;

	auto get_fullname() const -> const std::string&;

	auto get_full_declname() const -> const std::string&;

	auto get_type() const -> const mono_type&;

	auto get_visibility() const -> visibility;

	auto is_static() const -> bool;

private:
	void __generate_meta();
	auto __is_valuetype() const -> bool;

	template <typename T>
	void __set_value(const mono_object* obj, const T& val) const;

	template <typename T>
	auto __get_value(const mono_object* obj) const -> T;

	std::shared_ptr<mono_type> type_;

	non_owning_ptr<MonoClassField> field_ = nullptr;

	non_owning_ptr<MonoVTable> owning_type_vtable_ = nullptr;

	std::string name_;

	std::string fullname_;

	std::string full_declname_;
};

template <typename T>
void mono_field::set_value(const T& val) const
{
	__set_value(nullptr, val);
}

template <typename T>
void mono_field::set_value(const mono_object& object, const T& val) const
{
	__set_value(&object, val);
}

template <typename T>
void mono_field::__set_value(const mono_object* object, const T& val) const
{
	assert(field_);

	auto mono_val = convert_mono_type<T>::to_mono(val);
	auto arg = to_mono_arg(mono_val);

	if(object)
	{
		auto obj = object->get_internal_ptr();
		assert(obj);
		mono_field_set_value(obj, field_, arg);
	}
	else
	{
		mono_field_static_set_value(owning_type_vtable_, field_, arg);
	}
}

template <typename T>
auto mono_field::get_value() const -> T
{
	return __get_value<T>(nullptr);
}

template <typename T>
auto mono_field::get_value(const mono_object& object) const -> T
{
	return __get_value<T>(&object);
}

template <typename T>
auto mono_field::__get_value(const mono_object* object) const -> T
{
	T val{};

	assert(field_);
	MonoObject* refvalue = nullptr;
	auto arg = reinterpret_cast<void*>(&val);
	if(!__is_valuetype())
	{
		arg = &refvalue;
	}
	if(object)
	{
		auto obj = object->get_internal_ptr();
		assert(obj);
		mono_field_get_value(obj, field_, arg);
	}
	else
	{
		mono_field_static_get_value(owning_type_vtable_, field_, arg);
	}

	if(!__is_valuetype())
	{
		val = convert_mono_type<T>::from_mono(refvalue);
	}
	return val;
}

} // namespace mono
