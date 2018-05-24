#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_noncopyable.h"
#include <mono/metadata/metadata.h>

#include <unordered_map>

namespace mono
{

class mono_domain : public common::noncopyable
{
public:
	mono_domain(const std::string& name);
	~mono_domain();
	auto get_assembly(const std::string& path) const -> const mono_assembly&;

	auto get_internal_ptr() const -> MonoDomain*;

private:
	mutable std::unordered_map<std::string, mono_assembly> assemblies_;
	non_owning_ptr<MonoDomain> domain_ = nullptr;
};

} // namespace mono
