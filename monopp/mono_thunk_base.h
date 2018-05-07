#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include <mono/jit/jit.h>

namespace mono
{

class mono_thunk_base
{
public:
	mono_thunk_base() = default;

	explicit mono_thunk_base(mono_assembly* assembly, MonoMethod* method)
		: assembly_(assembly)
		, method_(method)
	{
	}

protected:
	mono_assembly* assembly_ = nullptr;
	MonoMethod* method_ = nullptr;
};

} // namespace mono
