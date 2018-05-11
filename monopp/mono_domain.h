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

	auto get_assembly(const std::string& path) -> mono_assembly&;
	auto get_internal_ptr() -> MonoDomain*;

private:
	MonoDomain* domain_ = nullptr;
	std::unordered_map<std::string, mono_assembly> assembies_;
};

} // namespace mono
