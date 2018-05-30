#include "mono_domain.h"
#include "mono_assembly.h"

#include "mono_string.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
END_MONO_INCLUDE

namespace mono
{
static const mono_domain* current_domain = nullptr;

auto mono_domain::get_internal_ptr() const -> MonoDomain*
{
	return domain_;
}

void mono_domain::set_current_domain(const mono_domain& domain)
{
	current_domain = &domain;
}

auto mono_domain::get_current_domain() -> const mono_domain&
{
	return *current_domain;
}

auto mono_domain::new_string(const std::string& str) const -> mono_string
{
	return mono_string(*this, str);
}

mono_domain::mono_domain(const std::string& name)
{
	domain_ = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);

	auto res = mono_domain_set(domain_, 0);
	if(res)
	{
		mono_thread_attach(domain_);
	}
}

mono_domain::~mono_domain()
{
	if(domain_)
	{
		auto root_domain = mono_get_root_domain();
		auto res = mono_domain_set(root_domain, 0);
		if(res)
		{
			mono_domain_unload(domain_);
		}
	}
	mono_gc_collect(mono_gc_max_generation());
}

auto mono_domain::get_assembly(const std::string& path) const -> mono_assembly
{
	auto it = assemblies_.find(path);
	if(it != assemblies_.end())
	{
		const auto& assembly = it->second;

		return assembly;
	}
	auto res = assemblies_.emplace(path, mono_assembly{*this, path});

	const auto& assembly = res.first->second;

	return assembly;
}

} // namespace mono
