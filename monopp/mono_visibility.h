#pragma once

#include "mono_config.h"

namespace mono
{
enum class visibility
{
	private_,
	protected_internal_,
	internal_,
	protected_,
	public_
};

inline std::string to_string(visibility vis)
{
	switch(vis)
	{
		case visibility::private_:
			return "private";
		case visibility::protected_internal_:
			return "protected internal";
		case visibility::internal_:
			return "internal";
		case visibility::protected_:
			return "protected";
		case visibility::public_:
			return "public";
	}
	return "private";
}

} // namespace mono
