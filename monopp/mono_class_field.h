#pragma once

#include "mono_config.h"

#include "mono_class.h"
#include "mono_domain.h"
#include "mono_object.h"
#include "mono_type_conversion.h"

namespace mono
{

class mono_class_field
{
public:
	explicit mono_class_field(const mono_class& cls, const std::string& name);

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
	auto get_visibility() const -> visibility;
	auto is_static() const -> bool;

	auto get_internal_ptr() const -> MonoClassField*;

private:
	void __generate_meta();
	template <typename T>
	void __set_value(const mono_object* obj, const T& val) const;
	template <typename T>
	auto __get_value(const mono_object* obj) const -> T;

	mono_class class_;
	non_owning_ptr<MonoClassField> field_ = nullptr;
	non_owning_ptr<MonoVTable> class_vtable_ = nullptr;
	std::string name_;
	std::string fullname_;
	std::string full_declname_;
};

template <typename T>
void mono_class_field::set_value(const T& val) const
{
	__set_value(nullptr, val);
}

template <typename T>
void mono_class_field::set_value(const mono_object& object, const T& val) const
{
	__set_value(&object, val);
}

template <typename T>
void mono_class_field::__set_value(const mono_object* object, const T& val) const
{
	assert(get_internal_ptr());

	auto mono_val = convert_mono_type<T>::to_mono(val);
	auto arg = to_mono_arg(mono_val);

	if(object)
	{
		auto obj = object->get_internal_ptr();
		assert(obj);
		mono_field_set_value(obj, get_internal_ptr(), arg);
	}
	else
	{
		mono_field_static_set_value(class_vtable_, get_internal_ptr(), arg);
	}
}

template <typename T>
auto mono_class_field::get_value() const -> T
{
	return __get_value<T>(nullptr);
}

template <typename T>
auto mono_class_field::get_value(const mono_object& object) const -> T
{
	return __get_value<T>(&object);
}

template <typename T>
auto mono_class_field::__get_value(const mono_object* object) const -> T
{
	T val{};

	assert(get_internal_ptr());
	MonoObject* refvalue = nullptr;
	auto arg = reinterpret_cast<void*>(&val);
	if(!class_.is_valuetype())
	{
		arg = &refvalue;
	}
	if(object)
	{
		auto obj = object->get_internal_ptr();
		assert(obj);
		mono_field_get_value(obj, get_internal_ptr(), arg);
	}
	else
	{
		mono_field_static_get_value(class_vtable_, get_internal_ptr(), arg);
	}

	if(!class_.is_valuetype())
	{
		val = convert_mono_type<T>::from_mono(refvalue);
	}
	return val;
}

} // namespace mono
