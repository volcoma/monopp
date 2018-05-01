#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_thunk_signature.h"
#include <mono/jit/jit.h>

namespace mono
{

template <typename return_type_t>
class mono_thunk_base;

template <typename return_type_t, typename... args_t>
class mono_thunk_base<return_type_t(args_t...)>
{
public:
	using signature = typename mono_thunk_signature<return_type_t(args_t...)>::type;

	mono_thunk_base() = default;

	explicit mono_thunk_base(mono_assembly& assembly, MonoMethod* method)
		: assembly_(&assembly)
		, method_(reinterpret_cast<signature>(mono_method_get_unmanaged_thunk(method)))
	{
	}

protected:
	mono_assembly* assembly_ = nullptr;
	signature method_ = nullptr;
};

} // namespace mono
