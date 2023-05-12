#pragma once

#include "mono_config.h"

#include "mono_assembly.h"

namespace mono
{

auto init(const std::string& domain, bool enable_debugging = false) -> bool;

void shutdown();

struct compiler_params
{
	// Specifies the format of the output assembly
	// Can be one of: exe, winexe, library, module
	std::string output_type = "library";

	// Specifies output assembly name
	std::string output_name;

    // Specifies the input files to compile
	std::vector<std::string> files;

    // Everything below is optional

	// Imports metadata from the specified assemblies
	std::vector<std::string> references;

	// Specifies the location of referenced assemblies
    std::vector<std::string> references_locations;

	// Defines one or more conditional symbols
	std::vector<std::string> defines;
};

auto create_compile_command(const compiler_params& params) -> std::string;
auto compile(const compiler_params& params) -> bool;

} // namespace mono
