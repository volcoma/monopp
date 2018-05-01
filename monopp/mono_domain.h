#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_noncopyable.h"
#include "mono_type_conversion.h"
#include <cassert>
#include <mono/jit/jit.h>
#include <string>
#include <unordered_map>

namespace mono
{

class mono_domain : public common::noncopyable
{
public:
    mono_domain();
    mono_domain(MonoDomain* domain);    
	~mono_domain();

	mono_assembly& get_assembly(const std::string& path);
    auto get_mono_domain_ptr() -> MonoDomain*;
private:
    MonoDomain* domain_ = nullptr;
    std::unordered_map<std::string, mono_assembly> assembies_;
};

} // namespace mono
