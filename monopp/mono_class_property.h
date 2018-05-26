#pragma once

#include "mono_config.h"

#include "mono_class.h"
#include "mono_domain.h"
#include "mono_object.h"
#include "mono_type_conversion.h"

namespace mono
{

class mono_class;

class mono_class_property
{
public:
	explicit mono_class_property(const mono_class& cls, const std::string& name);

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

	auto get_class() const -> const mono_class&;
	auto get_get_method() const -> mono_method;
	auto get_set_method() const -> mono_method;
	auto get_visibility() const -> visibility;
	auto is_static() const -> bool;

	auto get_internal_ptr() const -> MonoProperty*;

private:
	void __generate_meta();
	template <typename T>
	void __set_value(const mono_object* obj, const T& val) const;
	template <typename T>
	auto __get_value(const mono_object* obj) const -> T;

	mono_class class_;
	non_owning_ptr<MonoProperty> property_ = nullptr;
	std::string name_;
	std::string fullname_;
	std::string full_declname_;
};

template <typename T>
void mono_class_property::set_value(const T& val) const
{
	__set_value(nullptr, val);
}

template <typename T>
void mono_class_property::set_value(const mono_object& object, const T& val) const
{
	__set_value(&object, val);
}

template <typename T>
void mono_class_property::__set_value(const mono_object* object, const T& val) const
{
	assert(get_internal_ptr());
	MonoObject* ex = nullptr;
	auto mono_val = convert_mono_type<T>::to_mono(val);
	void* argsv[] = {to_mono_arg(mono_val)};
	MonoObject* obj = nullptr;
	if(object)
	{
		obj = object->get_internal_ptr();
		assert(obj);
	}
	mono_property_set_value(get_internal_ptr(), obj, argsv, &ex);

	if(ex)
	{
		throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
	}
}

template <typename T>
auto mono_class_property::get_value() const -> T
{
	return __get_value<T>(nullptr);
}

template <typename T>
auto mono_class_property::get_value(const mono_object& object) const -> T
{
	return __get_value<T>(&object);
}

template <typename T>
auto mono_class_property::__get_value(const mono_object* object) const -> T
{
	assert(get_internal_ptr());
	MonoObject* ex = nullptr;

	MonoObject* obj = nullptr;
	if(object)
	{

		obj = object->get_internal_ptr();
		assert(obj);
	}
	auto res = mono_property_get_value(get_internal_ptr(), obj, nullptr, &ex);

	if(ex)
	{
		throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
	}

	T val = convert_mono_type<T>::from_mono(res);

	return val;
}

} // namespace mono
