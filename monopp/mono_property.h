#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_method_thunk.h"

namespace mono
{

class mono_type;

class mono_property
{
public:
	explicit mono_property(const mono_type& type, const std::string& name);

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

	auto get_get_method() const -> mono_method;

	auto get_set_method() const -> mono_method;

	auto get_visibility() const -> visibility;

	auto is_static() const -> bool;

	auto get_internal_ptr() const -> MonoProperty*;

private:
	void __generate_meta();

	std::shared_ptr<mono_type> type_;

	non_owning_ptr<MonoProperty> property_ = nullptr;

	std::string name_;

	std::string fullname_;

	std::string full_declname_;
};

template <typename T>
void mono_property::set_value(const T& val) const
{
	auto thunk = make_thunk<void(const T&)>(get_set_method());
	thunk(val);
}

template <typename T>
void mono_property::set_value(const mono_object& object, const T& val) const
{
	auto thunk = make_thunk<void(const T&)>(get_set_method());
	thunk(object, val);
}

template <typename T>
auto mono_property::get_value() const -> T
{
	auto thunk = make_thunk<T()>(get_get_method());
	return thunk();
}

template <typename T>
auto mono_property::get_value(const mono_object& object) const -> T
{
	auto thunk = make_thunk<T()>(get_get_method());
	return thunk(object);
}

} // namespace mono
