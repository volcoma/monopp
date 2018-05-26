#pragma once

#include "mono_config.h"

#include <mono/metadata/image.h>

namespace mono
{

class mono_domain;
class mono_class;

class mono_assembly
{
public:
	explicit mono_assembly(const mono_domain& domain, const std::string& path);

	auto get_class(const std::string& name) const -> mono_class;
	auto get_class(const std::string& name_space, const std::string& name) const -> mono_class;

	auto valid() const -> bool;

	auto dump_references() const -> std::vector<std::string>;

private:
	non_owning_ptr<MonoAssembly> assembly_ = nullptr;
	non_owning_ptr<MonoImage> image_ = nullptr;
};

} // namespace mono
