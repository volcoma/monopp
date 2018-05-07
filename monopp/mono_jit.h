#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include "mono_type_conversion.h"
#include <cassert>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

namespace mono
{

class mono_assembly;

bool init(const std::string& domain, bool enable_debugging = false);
void shutdown();
auto get_auto_wrap_assembly() -> mono_assembly&;
void set_auto_wrap_assembly(const mono_assembly &assembly);

template <typename T>
inline void add_internal_call(const std::string& name, T&& func)
{
    mono_add_internal_call(name.c_str(), reinterpret_cast<const void*>(func));
}

template <typename signature_t, signature_t& signature>
struct mono_jit_internal_call_wrapper;

template <typename... args_t, void (&func)(args_t...)>
struct mono_jit_internal_call_wrapper<void(args_t...), func>
{
	static void wrapper(typename convert_mono_type<args_t>::mono_type_name... args)
	{
		func(convert_mono_type<args_t>::from_mono(std::move(args))...);
	}
};

template <typename return_t, typename... args_t, return_t (&func)(args_t...)>
struct mono_jit_internal_call_wrapper<return_t(args_t...), func>
{
	static typename convert_mono_type<return_t>::mono_type_name
	wrapper(typename convert_mono_type<args_t>::mono_type_name... args)
	{
		auto& internal_call_assembly = get_auto_wrap_assembly();
		assert(internal_call_assembly.valid() &&
			   "Internal call assembly not set. Call mono_jit::set_auto_wrap_assembly before execution.");
		return convert_mono_type<return_t>::to_mono(
			internal_call_assembly, func(convert_mono_type<args_t>::from_mono(std::move(args))...));
	}
};

/*!
 * Wrap a non-static function for mono::add_internal_call, where automatic type
 * converstion is done through convert_mono_type. Add your own specialisation implementation
 * of this class to support more types.
 */
#define mono_auto_wrap(func) &mono::mono_jit_internal_call_wrapper<decltype(func), func>::wrapper

} // namespace mono
