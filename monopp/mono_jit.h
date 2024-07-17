#pragma once

#include "mono_config.h"

#include "mono_assembly.h"

namespace mono
{

struct compiler_paths
{
	std::string assembly_dir;
	std::string config_dir;
	std::string msc_executable;
};

auto init(const compiler_paths& paths = {}, bool enable_debugging = false) -> bool;

void shutdown();

struct compiler_params
{
	std::string compiler_executable{};
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

auto get_common_library_names() -> const std::vector<std::string>&;
auto get_common_library_paths() -> const std::vector<std::string>&;

auto get_common_executable_names() -> const std::vector<std::string>&;
auto get_common_executable_paths() -> const std::vector<std::string>&;

auto get_common_config_paths() -> const std::vector<std::string>&;

} // namespace mono
