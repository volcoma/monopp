#pragma once

#include "mono_config.h"

#include <mono/metadata/image.h>

namespace mono
{

class mono_domain;
class mono_class;
class mono_class_instance;
class mono_string;

class mono_assembly
{
public:
	mono_assembly() = default;
	explicit mono_assembly(mono_domain* domain, const std::string& path);

	mono_assembly(const mono_assembly& o);
	auto operator=(const mono_assembly& o) -> mono_assembly&;

	mono_assembly(mono_assembly&& o);
	auto operator=(mono_assembly&& o) -> mono_assembly&;

	auto get_class(const std::string& name) -> mono_class;
	auto get_class(const std::string& name_space, const std::string& name) -> mono_class;

	auto new_class_instance(const mono_class& cls) const -> mono_class_instance;
	auto new_string(const std::string& str) const -> mono_string;

	auto valid() const -> bool;
	auto get_domain() const -> mono_domain*;

	auto get_internal_ptr() const -> MonoAssembly*;

private:
	mono_domain* domain_ = nullptr;
	MonoAssembly* assembly_ = nullptr;
	MonoImage* image_ = nullptr;
};

} // namespace mono
