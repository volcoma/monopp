#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include <mono/jit/jit.h>
#include <string>

namespace mono
{

class mono_domain;
class mono_class;
class mono_class_instance;
class mono_string;

class mono_assembly : public common::noncopyable
{
public:
	mono_assembly() = default;
	explicit mono_assembly(mono_domain* domain, MonoAssembly* assembly);
	explicit mono_assembly(mono_domain* domain, const std::string& path);

	mono_assembly(mono_assembly&& o);
	auto operator=(mono_assembly&& o) -> mono_assembly&;

	auto get_mono_assembly_ptr() const -> MonoAssembly*;
	auto get_mono_domain_ptr() const -> mono_domain*;

	auto get_class(const std::string& name) -> mono_class;
	auto get_class(const std::string& name_space, const std::string& name) -> mono_class;

	auto new_class_instance(const mono_class& cls) -> mono_class_instance;
	auto new_string(const std::string& str) const -> mono_string;

	auto valid() const -> bool;

private:
	mono_domain* domain_ = nullptr;
	MonoAssembly* assembly_ = nullptr;
	MonoImage* image_ = nullptr;
};

} // namespace mono
