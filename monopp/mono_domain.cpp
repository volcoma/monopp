#include "mono_domain.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_jit.h"

#include <mono_build_config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/threads.h>
extern "C" {
	#include <mono/metadata/mono-debug.h>
}
#include <mono/metadata/mono-gc.h>
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

auto mono_domain::get_mono_domain_ptr() -> MonoDomain*
{
    return domain_;
}

mono_domain::mono_domain(const std::string& name)
{
    domain_ = mono_domain_create_appdomain((char*)name.c_str(), nullptr);
    mono_domain_set (domain_, 0);
}

mono_domain::mono_domain(MonoDomain *domain)
{
    domain_ = domain;
}

mono_domain::~mono_domain()
{
    if(domain_)
    {
        mono_domain_set (mono_get_root_domain (), 0);
        //mono_domain_unload (domain_);
        mono_domain_free(domain_, false);

        //mono_gc_collect (mono_gc_max_generation ());
        //mono_domain_finalize(domain_, -1);
        //domain_ = nullptr;
    }
}

} // namespace mono
