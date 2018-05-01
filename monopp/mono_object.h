#pragma once

#include "mono_config.h"

#include <mono/jit/jit.h>

namespace mono
{

class mono_object
{
public:
	mono_object() = default;
	explicit mono_object(MonoObject* object);

	auto get_mono_object() const -> MonoObject*;

protected:
	MonoObject* object_ = nullptr;
};

} // namespace mono
