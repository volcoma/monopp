#pragma once

#include "mono_config.h"

#include "mono_visibility.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/class.h>
#include <mono/metadata/image.h>
END_MONO_INCLUDE

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
	void generate_meta();

	non_owning_ptr<MonoClass> class_ = nullptr;

	non_owning_ptr<MonoType> type_ = nullptr;

	std::string namespace_;

	std::string name_;

	std::string fullname_;

	std::uint32_t sizeof_ = 0;

	std::uint32_t alignof_ = 0;

	int rank_ = 0;

	bool valuetype_ = true;
};

} // namespace mono
