#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include "mono_object.h"
#include "mono_type.h"

namespace mono
{

class mono_object;
class mono_class;
class mono_assembly;

class mono_method : public common::noncopyable
{
public:
    explicit mono_method(const mono_class& cls, MonoMethod* method);
	explicit mono_method(const mono_class& cls, const std::string& name_with_args);
	explicit mono_method(const mono_class& cls, const std::string& name, int argc);
	mono_method(mono_method&& o) = default;

	auto get_owning_class() const -> const mono_class&;
	auto get_assembly() const -> const mono_assembly&;
    
    auto get_return_type() const -> mono_type;
	auto get_name() const -> const std::string&;
	auto get_fullname() const -> const std::string&;
    auto get_visibility() const -> visibility;

	auto get_internal_ptr() const -> MonoMethod*;

private:
	const mono_class& class_;
	non_owning_ptr<MonoMethod> method_ = nullptr;
	std::string name_;
	std::string fullname_;
};

} // namespace mono
