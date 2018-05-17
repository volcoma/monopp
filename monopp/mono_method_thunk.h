#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_method.h"
#include "mono_object.h"
#include "mono_string.h"
#include "mono_type_conversion.h"

#include <tuple>
#include <utility>
#include <vector>

namespace mono
{

template <typename return_type_t>
class mono_method_thunk;

template <typename... args_t>
class mono_method_thunk<void(args_t...)> : public mono_method
{
public:
	explicit mono_method_thunk(mono_method&& m)
		: mono_method(std::move(m))
	{
	}

	void operator()(args_t... args)
	{
		auto method = this->method_;
		auto object = this->object_;
		auto tup = std::make_tuple(
			convert_mono_type<args_t>::to_mono(*this->assembly_, std::forward<args_t>(args))...);

		auto inv = [method, object](auto... args) {
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			mono_runtime_invoke(method, object, argsv.data(), &ex);
			if(ex)
			{
				throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
			}

		};

		apply(inv, tup);
	}
};

template <typename return_type_t, typename... args_t>
class mono_method_thunk<return_type_t(args_t...)> : public mono_method
{
public:
	explicit mono_method_thunk(mono_method&& m)
		: mono_method(std::move(m))
	{
	}

	auto operator()(args_t... args)
	{
		auto method = this->method_;
		auto object = this->object_;
		auto tup = std::make_tuple(
			convert_mono_type<args_t>::to_mono(*this->assembly_, std::forward<args_t>(args))...);
		auto inv = [method, object](auto... args) {
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			auto result = mono_runtime_invoke(method, object, argsv.data(), &ex);
			if(ex)
			{
				throw mono_thunk_exception(reinterpret_cast<MonoException*>(ex));
			}

			return result;
		};

		auto result = apply(inv, tup);
		return convert_mono_type<return_type_t>::from_mono(std::move(result));
	}
};

} // namespace mono
