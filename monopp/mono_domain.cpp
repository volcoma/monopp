#include "mono_domain.h"
#include "mono_assembly.h"

#include "mono_jit.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>

namespace mono
{

auto mono_domain::get_internal_ptr() const -> MonoDomain*
{
	return domain_;
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

auto mono_domain::get_assembly(const std::string& path) const -> const mono_assembly&
{
	auto it = assemblies_.find(path);
	if(it != assemblies_.end())
	{
		const auto& assembly = it->second;

		set_auto_wrap_assembly(assembly);

		return assembly;
	}
	auto res = assemblies_.emplace(path, mono_assembly{*this, path});

	const auto& assembly = res.first->second;

	set_auto_wrap_assembly(assembly);

	return assembly;
}

} // namespace mono
