#pragma once

#include "mono_config.h"

#include "mono_type.h"
#include "mono_visibility.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/object.h>
END_MONO_INCLUDE

namespace mono
{

class mono_object;

class mono_property
{
public:
	explicit mono_property(const mono_type& type, const std::string& name);

	auto get_name() const -> std::string;

	auto get_fullname() const -> std::string;

	auto get_full_declname() const -> std::string;

	auto get_type() const -> const mono_type&;

	auto get_get_method() const -> mono_method;

	auto get_set_method() const -> mono_method;

	auto get_visibility() const -> visibility;

	auto is_static() const -> bool;

	auto is_readonly() const -> bool;

	auto get_attributes() const -> std::vector<mono_object>;

	auto is_special_name() const -> bool;

	auto has_default() const -> bool;

	auto get_internal_ptr() const -> MonoProperty*;

private:
	void generate_meta();

	mono_type type_;

	non_owning_ptr<MonoProperty> property_ = nullptr;

#ifndef NDEBUG
	struct meta_info
	{
		std::string name;
		std::string fullname;
		std::string full_declname;
	};

	std::shared_ptr<meta_info> meta_{};
#endif
};

} // namespace mono
