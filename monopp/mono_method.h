#pragma once

#include "mono_config.h"

#include "mono_object.h"
#include "mono_visibility.h"

namespace mono
{

class mono_object;
class mono_type;

class mono_method
{
public:
	explicit mono_method(MonoMethod* method);
	explicit mono_method(const mono_type& type, const std::string& name_with_args);
	explicit mono_method(const mono_type& type, const std::string& name, int argc);

	auto get_return_type() const -> mono_type;
	auto get_param_types() const -> std::vector<mono_type>;
	auto get_name() const -> const std::string&;
	auto get_fullname() const -> const std::string&;
	auto get_full_declname() const -> const std::string&;
	auto get_visibility() const -> visibility;
	auto is_static() const -> bool;
	auto is_virtual() const -> bool;

protected:
	void generate_meta();

	non_owning_ptr<MonoMethod> method_ = nullptr;
	non_owning_ptr<MonoMethodSignature> signature_ = nullptr;
	std::string name_;
	std::string fullname_;
	std::string full_declname_;
};

} // namespace mono
