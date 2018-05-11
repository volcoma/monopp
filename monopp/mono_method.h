#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include "mono_object.h"

#include <mono/metadata/metadata.h>

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

    auto get_internal_ptr() const -> MonoMethod*;

protected:
	mono_assembly* assembly_ = nullptr;
	MonoObject* object_ = nullptr;
	MonoMethod* method_ = nullptr;
};

} // namespace mono
