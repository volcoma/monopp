#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include "mono_thunk.h"
#include <mono/jit/jit.h>
#include <string>
#include <vector>

namespace mono
{

class mono_assembly;

class mono_static_function : public common::noncopyable
{
public:
	mono_static_function() = default;
	explicit mono_static_function(mono_assembly* assembly, MonoClass* cls, const std::string& name, int argc);

	mono_static_function(mono_static_function&& o);
	auto operator=(mono_static_function&& o) -> mono_static_function&;

	template <typename function_signature_t>
	auto get_thunk();

private:
    mono_assembly* assembly_ = nullptr;
	MonoMethod* method_ = nullptr;
};

template <typename function_signature_t>
auto mono_static_function::get_thunk()
{
	return mono_thunk<function_signature_t>(*assembly_, method_);
}

} // namespace mono
