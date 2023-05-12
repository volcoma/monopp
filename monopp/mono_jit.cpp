#include "mono_jit.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_logger.h"

BEGIN_MONO_INCLUDE
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>
#include <mono/utils/mono-logger.h>
#include <mono_build_config.h>
END_MONO_INCLUDE

#include <iostream>

namespace mono
{

static void on_log_callback(const char* log_domain, const char* log_level, const char* message,
							mono_bool /*fatal*/, void* /*user_data*/)

{

	//	static const char* mono_error_levels[] = {"error", "critical", "warning",
	//											"message", "info",  "debug"};

	std::string category;
	if(log_level == nullptr)
	{
		category = "warning";
	}
	std::string format_msg;
	if(log_domain)
	{
		format_msg += "[";
		format_msg += log_domain;
		format_msg += "] ";
	}
	format_msg += message;
	const auto& logger = get_log_handler(category);
	if(logger)
	{
		logger(format_msg);
	}
	else
	{
		const auto& default_logger = get_log_handler("default");
		if(default_logger)
		{
			default_logger(format_msg);
		}
	}
}

static MonoDomain* jit_domain = nullptr;

auto init(const std::string& domain, bool enable_debugging) -> bool
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

	mono_trace_set_level_string("warning");
	mono_trace_set_log_handler(on_log_callback, nullptr);

	set_log_handler("default", [](const std::string& msg) { std::cout << msg << std::endl; });

	jit_domain = mono_jit_init(domain.c_str());
	mono_thread_set_main(mono_thread_current());

	return jit_domain != nullptr;
}

void shutdown()
{
	if(jit_domain)
	{
		mono_jit_cleanup(jit_domain);
	}
	jit_domain = nullptr;
}

auto quote(const std::string& word) -> std::string
{
	std::string command;
	command.append("\"");
	command.append(word);
	command.append("\"");
	return command;
}


auto create_compile_command(const compiler_params& params) -> std::string
{
	std::string command;
	command.append(quote(INTERNAL_MONO_MCS_EXECUTABLE));

	for(const auto& path : params.files)
	{
		command += " ";
		command += quote(path);
	}

    if(!params.output_type.empty())
    {
        command += " -target:";
        command += params.output_type;
    }

    if(!params.references.empty())
    {
        command += " -reference:";

        for(const auto& ref : params.references)
        {
            command += ref;
            command += ",";
        }

        command.pop_back();
    }

    if(!params.references_locations.empty())
    {
        command += " -lib:";

        for(const auto& loc : params.references_locations)
        {
            command += loc;
            command += ",";
        }

        command.pop_back();
    }

#ifndef NDEBUG
    command += " -debug";
#else
    command += " -optimize";
#endif
	command += " -out:";
	command += quote(params.output_name);

#ifdef _WIN32
	command = quote(command);
#endif
    return command;
}

auto compile(const compiler_params& params) -> bool
{
	auto command = create_compile_command(params);
    std::cout << command << std::endl;
	return std::system(command.c_str()) == 0;
}


} // namespace mono
