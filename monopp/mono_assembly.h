#pragma once

#include "mono_config.h"

#include <mono/metadata/image.h>

namespace mono
{

class mono_domain;
class mono_type;

class mono_assembly
{
public:
	explicit mono_assembly(const mono_domain& domain, const std::string& path);

	auto get_type(const std::string& name) const -> mono_type;

	auto get_type(const std::string& name_space, const std::string& name) const -> mono_type;

	auto dump_references() const -> std::vector<std::string>;

private:
	non_owning_ptr<MonoAssembly> assembly_ = nullptr;
	non_owning_ptr<MonoImage> image_ = nullptr;
};

} // namespace mono
