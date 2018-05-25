#include "mono_jit.h"
#include "mono_assembly.h"
#include "mono_exception.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>
#include <mono_build_config.h>

namespace mono
{
namespace detail
{
static MonoDomain* jit_domain_ = nullptr;
} // namespace detail

bool init(const std::string& domain, bool enable_debugging)
{
	mono_set_dirs(INTERNAL_MONO_ASSEMBLY_DIR, INTERNAL_MONO_CONFIG_DIR);

	if(enable_debugging)
	{
		// clang-format off
		const char* options[] =
		{
			"--soft-breakpoints",
			"--debugger-agent=transport=dt_socket,suspend=n,server=y,address=127.0.0.1:55555,embedding=1",
			"--debug-domain-unload",

			// GC options:
			// check-remset-consistency: Makes sure that write barriers are properly issued in native code,
			// and therefore
			//    all old->new generation references are properly present in the remset. This is easy to mess
			//    up in native code by performing a simple memory copy without a barrier, so it's important to
			//    keep the option on.
			// verify-before-collections: Unusure what exactly it does, but it sounds like it could help track
			// down
			//    things like accessing released/moved objects, or attempting to release handles for an
			//    unloaded domain.
			// xdomain-checks: Makes sure that no references are left when a domain is unloaded.
			"--gc-debug=check-remset-consistency,verify-before-collections,xdomain-checks"
		};
		// clang-format on
		mono_jit_parse_options(sizeof(options) / sizeof(char*), const_cast<char**>(options));
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
	}

	detail::jit_domain_ = mono_jit_init(domain.c_str());

	mono_thread_set_main(mono_thread_current());

	return detail::jit_domain_ != nullptr;
}

void shutdown()
{
	if(detail::jit_domain_)
	{
		mono_jit_cleanup(detail::jit_domain_);
	}
	detail::jit_domain_ = nullptr;
}

} // namespace mono
