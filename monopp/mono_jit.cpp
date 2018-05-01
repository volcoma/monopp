#include "mono_jit.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include <mono_build_config.h>

namespace mono
{
namespace detail
{
static MonoDomain* jit_domain_ = nullptr;
static mono_assembly internal_call_assembly_;
}

bool init(const std::string& domain)
{
	mono_set_dirs(INTERNAL_MONO_ASSEMBLY_DIR, INTERNAL_MONO_CONFIG_DIR);

	detail::jit_domain_ = mono_jit_init(domain.c_str());

	return detail::jit_domain_ != nullptr;
}

void shutdown()
{
	try
	{
        if(detail::jit_domain_)
            mono_jit_cleanup(detail::jit_domain_);
        
        detail::jit_domain_ = nullptr;
	}
	catch(...)
	{
        int a = 0;
        a++;
	}
	

}

void set_auto_wrap_assembly(const mono_assembly& assembly)
{
	detail::internal_call_assembly_ =
		mono_assembly(assembly.get_mono_domain_ptr(), assembly.get_mono_assembly_ptr());
}

auto get_auto_wrap_assembly() -> mono_assembly&
{
	return detail::internal_call_assembly_;
}

} // namespace mono
