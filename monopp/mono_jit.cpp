#include "mono_jit.h"
#include "mono_assembly.h"
#include "mono_exception.h"
//#include <mono/metadata/mono-debug.h>
#include <mono_build_config.h>

namespace mono
{
namespace detail
{
static MonoDomain* jit_domain_ = nullptr;
static mono_assembly internal_call_assembly_;
static bool debugging_enabled_ = false;
}

bool init(const std::string& domain, bool enable_debugging)
{
	mono_set_dirs(INTERNAL_MONO_ASSEMBLY_DIR, INTERNAL_MONO_CONFIG_DIR);

	detail::debugging_enabled_ = enable_debugging;

//	if(detail::debugging_enabled_)
//	{
//		const char* options[] = {
//			"--soft-breakpoints",
//			"--debugger-agent=transport=dt_socket,server=y,suspend=n,address=127.0.0.1:10000"};
//		mono_jit_parse_options(sizeof(options) / sizeof(char*),
//							   const_cast<char**>(reinterpret_cast<const char**>(options)));
//		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
//	}

	detail::jit_domain_ = mono_jit_init(domain.c_str());

	return detail::jit_domain_ != nullptr;
}

void shutdown()
{
	try
	{
//		if(detail::debugging_enabled_)
//		{
//			mono_debug_cleanup();
//		}
		if(detail::jit_domain_)
		{
			mono_jit_cleanup(detail::jit_domain_);
        }
        
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
	detail::internal_call_assembly_ = assembly;
}

auto get_auto_wrap_assembly() -> mono_assembly&
{
	return detail::internal_call_assembly_;
}

} // namespace mono
