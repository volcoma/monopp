#pragma once

#include "mono_type_conversion.h"

namespace mono
{

template <typename return_type_t>
class mono_thunk_signature;

template <typename... args_t>
class mono_thunk_signature<void(args_t...)>
{
public:
	using type = void (*)(typename convert_mono_type<args_t>::mono_type_name..., MonoException** ex);
};

template <typename return_type_t, typename... args_t>
class mono_thunk_signature<return_type_t(args_t...)>
{
public:
	using type = typename convert_mono_type<return_type_t>::mono_type_name (*)(
		typename convert_mono_type<args_t>::mono_type_name..., MonoException** ex);
};

} // namespace mono
