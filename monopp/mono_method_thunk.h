#pragma once

#include "mono_config.h"

#include "mono_exception.h"
#include "mono_method.h"
#include "mono_object.h"
#include "mono_type.h"
#include "mono_type_conversion.h"

#include <tuple>
#include <utility>
#include <vector>

namespace mono
{

template <typename T>
bool is_compatible_type(const mono_type& type)
{
	const auto& expected_name = type.get_fullname();
	bool found = false;

	auto name = types::get_name<T>(found).fullname;

	if(found)
	{
		return name == expected_name;
	}

	return true;
}

template <typename signature_t>
bool has_compatible_signature(const mono_method& method)
{
	constexpr auto arity = function_traits<signature_t>::arity;
	using return_type = typename function_traits<signature_t>::return_type;
	using arg_types = typename function_traits<signature_t>::arg_types_decayed;
	auto expected_rtype = method.get_return_type();
	auto expected_arg_types = method.get_param_types();

	bool compatible = arity == expected_arg_types.size();
	if(!compatible)
	{
		return false;
	}
	compatible &= is_compatible_type<return_type>(expected_rtype);
	if(!compatible)
	{
		// allow cpp return type to be void i.e ignoring it.
		if(!is_compatible_type<void>(expected_rtype))
		{
			return false;
		}
	}
	arg_types tuple;
	size_t idx = 0;
	for_each(tuple, [&compatible, &idx, &expected_arg_types](const auto& arg) {
        ignore(arg);
		auto expected_arg_type = expected_arg_types[idx];
		using arg_type = decltype(arg);
		compatible &= is_compatible_type<arg_type>(expected_arg_type);

		idx++;
	});

	return compatible;
}

template <typename return_type_t>
class mono_method_thunk;

template <typename... args_t>
class mono_method_thunk<void(args_t...)> : public mono_method
{
public:
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
		auto method = this->method_;
		MonoObject* object = nullptr;
		if(obj)
		{
			object = obj->get_internal_ptr();
			method = mono_object_get_virtual_method(object, method);
		}
		auto tup =
			std::make_tuple(convert_mono_type<std::decay_t<args_t>>::to_mono(std::forward<args_t>(args))...);

		auto inv = [method, object](auto... args) {
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			mono_runtime_invoke(method, object, argsv.data(), &ex);
			if(ex)
			{
				throw mono_thunk_exception(ex);
			}
		};

		apply(inv, tup);
	}

	template <typename signature_t>
	friend mono_method_thunk<signature_t> make_thunk(mono_method&&, bool);

	mono_method_thunk(mono_method&& o)
		: mono_method(std::move(o))
	{
	}
};

template <typename return_type_t, typename... args_t>
class mono_method_thunk<return_type_t(args_t...)> : public mono_method
{
public:
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
		auto method = this->method_;
		MonoObject* object = nullptr;
		if(obj)
		{
			object = obj->get_internal_ptr();
			method = mono_object_get_virtual_method(object, method);
		}
		auto tup =
			std::make_tuple(convert_mono_type<std::decay_t<args_t>>::to_mono(std::forward<args_t>(args))...);
		auto inv = [method, object](auto... args) {
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			auto result = mono_runtime_invoke(method, object, argsv.data(), &ex);
			if(ex)
			{
				throw mono_thunk_exception(ex);
			}

			return result;
		};

		auto result = apply(inv, tup);
		return convert_mono_type<std::decay_t<return_type_t>>::from_mono_boxed(std::move(result));
	}

	template <typename signature_t>
	friend mono_method_thunk<signature_t> make_thunk(mono_method&&, bool);

	mono_method_thunk(mono_method&& o)
		: mono_method(std::move(o))
	{
	}
};

template <typename signature_t>
mono_method_thunk<signature_t> make_thunk(mono_method&& method, bool check_signature = true)
{
	if(check_signature && !has_compatible_signature<signature_t>(method))
	{
		throw mono_exception("NATIVE::Method thunk requested with incompatible signature");
	}
	return mono_method_thunk<signature_t>(std::move(method));
}

template <typename signature_t>
mono_method_thunk<signature_t> make_thunk(const mono_type& type, const std::string& name)
{
	using arg_types = typename function_traits<signature_t>::arg_types;
	arg_types tup;
	auto args_result = types::get_args_signature(tup);
	auto args = args_result.first;
	auto all_types_known = args_result.second;

	if(all_types_known)
	{
		auto func = type.get_method(name + "(" + args + ")");
		return make_thunk<signature_t>(std::move(func));
	}
	else
	{
		constexpr auto arg_count = function_traits<signature_t>::arity;
		auto func = type.get_method(name, arg_count);
		return make_thunk<signature_t>(std::move(func));
	}
}

} // namespace mono
