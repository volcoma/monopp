#include "mono_domain.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_jit.h"

#include <mono_build_config.h>
#include <mono/metadata/assembly.h>

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

mono_domain::mono_domain()
{
    domain_ = mono_domain_create();    
}

mono_domain::mono_domain(MonoDomain *domain)
{
    domain_ = domain;
}

mono_domain::~mono_domain()
{
    if(domain_)
    {
        mono_domain_free(domain_, true);
        domain_ = nullptr;
    }
}

} // namespace mono
