#pragma once

#include "mono_config.h"
#include "mono_type_traits.h"

#include "mono_noncopyable.h"
#include "mono_static_function.h"
#include <mono/jit/jit.h>
#include <string>

namespace mono
{

class mono_assembly;
class mono_class_field;
class mono_class_property;

class mono_class : public common::noncopyable
{
public:
	mono_class() = default;
	explicit mono_class(mono_assembly* assembly, MonoClass* cls);
	explicit mono_class(mono_assembly* assembly, MonoImage* image, const std::string& name);
	explicit mono_class(mono_assembly* assembly, MonoImage* image, const std::string& name_space,
						const std::string& name);

	mono_class(mono_class&& o);
	auto operator=(mono_class&& o) -> mono_class&;

	auto get_static_function(const std::string& name, int argc = 0) const -> mono_static_function;

	template <typename function_signature_t>
	auto get_static_function_thunk(const std::string& name);

	auto get_mono_class_ptr() const -> MonoClass*;

	auto get_field(const std::string& name) const -> mono_class_field;
    auto get_property(const std::string& name) const -> mono_class_property;

	auto get_name() const -> std::string;

private:
	mono_assembly* assembly_ = nullptr;
	MonoClass* class_ = nullptr;
};

template <typename function_signature_t>
auto mono_class::get_static_function_thunk(const std::string& name)
{
	constexpr auto arg_count = function_traits<function_signature_t>::arity;
	auto func = get_static_function(name, arg_count);
	return func.template get_thunk<function_signature_t>();
}

} // namespace mono
