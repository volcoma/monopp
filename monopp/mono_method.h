#pragma once

#include "mono_config.h"

#include "mono_method_thunk.h"
#include "mono_noncopyable.h"
#include <mono/jit/jit.h>
#include <string>
#include <vector>

namespace mono
{

class mono_object;
class mono_assembly;

class mono_method : public common::noncopyable
{
public:
	mono_method() = default;
	explicit mono_method(mono_assembly* assembly, MonoClass* cls, MonoObject* object, const std::string& name,
						 int argc);

	mono_method(mono_method&& o);
	auto operator=(mono_method&& o) -> mono_method&;

	template <typename function_signature_t>
	auto get_thunk();

private:
	mono_assembly* assembly_ = nullptr;
    MonoObject* object_ = nullptr;
    MonoMethod* method_ = nullptr;
};

template <typename function_signature_t>
auto mono_method::get_thunk()
{
	return mono_method_thunk<function_signature_t>(*assembly_, object_, method_);
}

} // namespace mono
