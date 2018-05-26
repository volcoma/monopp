#pragma once

#include "mono_config.h"

#include "mono_object.h"
#include "mono_visibility.h"

namespace mono
{

class mono_object;
class mono_class;

class mono_method
{
public:
	explicit mono_method(MonoMethod* method);
	explicit mono_method(const mono_class& cls, const std::string& name_with_args);
	explicit mono_method(const mono_class& cls, const std::string& name, int argc);

	auto get_return_class() const -> mono_class;
	auto get_name() const -> const std::string&;
	auto get_fullname() const -> const std::string&;
	auto get_full_declname() const -> const std::string&;
	auto get_visibility() const -> visibility;
	auto is_static() const -> bool;

	auto get_internal_ptr() const -> MonoMethod*;

private:
	void __generate_meta();

	non_owning_ptr<MonoMethod> method_ = nullptr;
	non_owning_ptr<MonoMethodSignature> signature_ = nullptr;
	std::string name_;
	std::string fullname_;
	std::string full_declname_;
};

} // namespace mono
