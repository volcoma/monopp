#pragma once

#include "mono_config.h"

#include "mono_assembly.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/metadata.h>
END_MONO_INCLUDE

#include <unordered_map>

namespace mono
{
class mono_string;

class mono_domain
{
public:
	explicit mono_domain(const std::string& name);

	~mono_domain();

	auto get_assembly(const std::string& path) const -> mono_assembly;

	auto new_string(const std::string& str) const -> mono_string;

	static void set_current_domain(const mono_domain& domain);

	static auto get_current_domain() -> const mono_domain&;

	auto get_internal_ptr() const -> MonoDomain*;

private:
	mutable std::unordered_map<std::string, mono_assembly> assemblies_;
	non_owning_ptr<MonoDomain> domain_ = nullptr;
};

} // namespace mono
