#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_string.h"
#include "mono_thunk_base.h"
#include "mono_type_conversion.h"
#include <mono/jit/jit.h>
#include <string>
#include <utility>

namespace mono
{

template <typename return_type_t>
class mono_thunk;

template <typename... args_t>
class mono_thunk<void(args_t...)> : public mono_thunk_base<void(args_t...)>
{
public:
	mono_thunk()
		: mono_thunk_base<void(args_t...)>()
		, mono_object()
	{
	}

	explicit mono_thunk(mono_assembly& assembly, MonoMethod* method)
		: mono_thunk_base<void(args_t...)>(assembly, method)
	{
	}

	void operator()(args_t... args)
	{
		MonoException* ex = nullptr;
		this->method_(convert_mono_type<args_t>::to_mono(*this->assembly_, std::forward<args_t>(args))...,
					  &ex);

		if(ex)
			throw mono_thunk_exception(ex);
	}
};

template <typename return_type_t, typename... args_t>
class mono_thunk<return_type_t(args_t...)> : public mono_thunk_base<return_type_t(args_t...)>
{
public:
	mono_thunk()
		: mono_thunk_base<return_type_t(args_t...)>()
		, mono_object()
	{
	}

	explicit mono_thunk(mono_assembly& assembly, MonoMethod* method)
		: mono_thunk_base<return_type_t(args_t...)>(assembly, method)
	{
	}

	auto operator()(args_t... args)
	{
		MonoException* ex = nullptr;
		auto result = this->method_(
			convert_mono_type<args_t>::to_mono(*this->assembly_, std::forward<args_t>(args))..., &ex);

		if(ex)
			throw mono_thunk_exception(ex);

		return convert_mono_type<return_type_t>::from_mono(std::move(result));
	}
};

} // namespace mono
