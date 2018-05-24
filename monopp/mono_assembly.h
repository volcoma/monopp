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
	explicit mono_assembly(const mono_domain& domain, const std::string& path);

	auto get_class(const std::string& name) const -> mono_class;
	auto get_class(const std::string& name_space, const std::string& name) const -> mono_class;

	auto new_instance(const mono_class& cls) const -> mono_class_instance;
	auto new_string(const std::string& str) const -> mono_string;

	auto valid() const -> bool;

	auto get_internal_ptr() const -> MonoAssembly*;
	auto get_image_ptr() const -> MonoImage*;
	auto get_domain() const -> const mono_domain&;

private:
	const mono_domain& domain_;

	non_owning_ptr<MonoAssembly> assembly_ = nullptr;
	non_owning_ptr<MonoImage> image_ = nullptr;
};

} // namespace mono
