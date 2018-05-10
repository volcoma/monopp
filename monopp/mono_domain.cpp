#include "mono_domain.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_jit.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono_build_config.h>
namespace mono
{

mono_assembly& mono_domain::get_assembly(const std::string& path)
{
	auto it = assembies_.find(path);
	if(it != assembies_.end())
	{
		return it->second;
	}
	assembies_.emplace(path, mono_assembly{this, path});

	auto& assembly = assembies_[path];

	set_auto_wrap_assembly(assembly);

	return assembies_[path];
}

auto mono_domain::get_internal_ptr() -> MonoDomain*
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
			mono_gc_collect(mono_gc_max_generation());
		}
	}
}

} // namespace mono
