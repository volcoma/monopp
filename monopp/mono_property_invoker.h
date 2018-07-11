#pragma once
#include "mono_property.h"

#include "mono_method_invoker.h"

namespace mono
{

template <typename T>
class mono_property_invoker : public mono_property
{
public:
	void set_value(const T& val) const;

	void set_value(const mono_object& obj, const T& val) const;

	auto get_value() const -> T;

	auto get_value(const mono_object& obj) const -> T;

private:
	template <typename Signature>
	friend mono_property_invoker<Signature> make_property_invoker(const mono_property&);

	explicit mono_property_invoker(const mono_property& property)
		: mono_property(property)
	{
	}
};

template <typename T>
void mono_property_invoker<T>::set_value(const T& val) const
{
	auto thunk = make_method_invoker<void(const T&)>(get_set_method());
	thunk(val);
}

template <typename T>
void mono_property_invoker<T>::set_value(const mono_object& object, const T& val) const
{
	auto thunk = make_method_invoker<void(const T&)>(get_set_method());
	thunk(object, val);
}

template <typename T>
auto mono_property_invoker<T>::get_value() const -> T
{
	auto thunk = make_method_invoker<T()>(get_get_method());
	return thunk();
}

template <typename T>
auto mono_property_invoker<T>::get_value(const mono_object& object) const -> T
{
	auto thunk = make_method_invoker<T()>(get_get_method());
	return thunk(object);
}

template <typename T>
mono_property_invoker<T> make_property_invoker(const mono_property& property)
{
	// const auto& expected_name = type.get_fullname();
	// bool compatible = types::is_compatible_type<T>(expected_name);
	return mono_property_invoker<T>(property);
}

template <typename T>
mono_property_invoker<T> make_property_invoker(const mono_type& type, const std::string& name)
{
	auto property = type.get_property(name);
	return make_property_invoker<T>(property);
}
} // namespace mono
