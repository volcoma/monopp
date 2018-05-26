#pragma once

#include "mono_config.h"

#include "mono_exception.h"
#include "mono_method.h"
#include "mono_object.h"
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
	mono_method_thunk(mono_method&& o)
		: mono_method(std::move(o))
	{
	}

	void operator()(args_t... args)
	{
		invoke(nullptr, std::forward<args_t>(args)...);
	}

	void operator()(const mono_object& obj, args_t... args)
	{
		invoke(&obj, std::forward<args_t>(args)...);
	}

private:
	void invoke(const mono_object* obj, args_t... args)
	{
		auto method = this->get_internal_ptr();
		MonoObject* object = nullptr;
		if(obj)
		{
			object = obj->get_internal_ptr();
			method = mono_object_get_virtual_method(object, method);
		}
		auto tup = std::make_tuple(convert_mono_type<args_t>::to_mono(std::forward<args_t>(args))...);

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
	mono_method_thunk(mono_method&& o)
		: mono_method(std::move(o))
	{
	}

	auto operator()(args_t... args)
	{
		return invoke(nullptr, std::forward<args_t>(args)...);
	}

	auto operator()(const mono_object& obj, args_t... args)
	{
		return invoke(&obj, std::forward<args_t>(args)...);
	}

private:
	auto invoke(const mono_object* obj, args_t... args)
	{
		auto method = this->get_internal_ptr();
		MonoObject* object = nullptr;
		if(obj)
		{
			object = obj->get_internal_ptr();
			method = mono_object_get_virtual_method(object, method);
		}
		auto tup = std::make_tuple(convert_mono_type<args_t>::to_mono(std::forward<args_t>(args))...);
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
