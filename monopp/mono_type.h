#pragma once

#include "mono_config.h"

#include "mono_method_thunk.h"
#include "mono_visibility.h"

#include <mono/metadata/class.h>
#include <mono/metadata/image.h>

namespace mono
{

class mono_assembly;
class mono_method;
class mono_field;
class mono_property;
class mono_object;

class mono_type
{
public:
	mono_type();

	explicit mono_type(MonoImage* image, const std::string& name);
	explicit mono_type(MonoImage* image, const std::string& name_space, const std::string& name);
	explicit mono_type(MonoClass* cls);
	explicit mono_type(MonoType* type);

	auto valid() const -> bool;

	auto new_instance() const -> mono_object;

	template <typename function_signature_t>
	auto get_method(const std::string& name);

	auto get_method(const std::string& name_with_args) const -> mono_method;

	auto get_method(const std::string& name, int argc) const -> mono_method;

	auto get_field(const std::string& name) const -> mono_field;

	auto get_property(const std::string& name) const -> mono_property;

	auto get_fields() const -> std::vector<mono_field>;

	auto get_properties() const -> std::vector<mono_property>;

	auto get_methods() const -> std::vector<mono_method>;

	auto has_base_type() const -> bool;

	auto get_base_type() const -> mono_type;

	auto get_nested_types() const -> std::vector<mono_type>;

	auto is_derived_from(const mono_type& type) const -> bool;

	auto get_namespace() const -> const std::string&;

	auto get_name() const -> const std::string&;

	auto get_fullname() const -> const std::string&;

	auto is_valuetype() const -> bool;

	auto get_rank() const -> int;

	auto get_sizeof() const -> std::uint32_t;

	auto get_alignof() const -> std::uint32_t;

	auto get_internal_ptr() const -> MonoClass*;

private:
	void __generate_meta();

	non_owning_ptr<MonoClass> class_ = nullptr;

	non_owning_ptr<MonoType> type_ = nullptr;

	std::string namespace_;

	std::string name_;

	std::string fullname_;

	int rank_ = 0;

	bool valuetype_ = true;

	std::uint32_t sizeof_ = 0;

	std::uint32_t alignof_ = 0;
};

template <typename function_signature_t>
auto mono_type::get_method(const std::string& name)
{
	using arg_types = typename function_traits<function_signature_t>::arg_types;
	arg_types tup;
	auto args_result = types::get_args_signature(tup);
	auto args = args_result.first;
	auto all_types_known = args_result.second;

	if(all_types_known)
	{
		auto func = get_method(name + "(" + args + ")");
		return mono_method_thunk<function_signature_t>(std::move(func));
	}
	else
	{
		constexpr auto arg_count = function_traits<function_signature_t>::arity;
		auto func = get_method(name, arg_count);
		return mono_method_thunk<function_signature_t>(std::move(func));
	}
}

} // namespace mono
