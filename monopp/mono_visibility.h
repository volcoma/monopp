#pragma once

#include "mono_config.h"

namespace mono
{
enum class visibility
{
	vis_private,
	vis_protected_internal,
	vis_internal,
	vis_protected,
	vis_public
};

inline std::string to_string(visibility vis)
{
	switch(vis)
	{
		case visibility::vis_private:
			return "private";
		case visibility::vis_protected_internal:
			return "protected internal";
		case visibility::vis_internal:
			return "internal";
		case visibility::vis_protected:
			return "protected";
		case visibility::vis_public:
			return "public";
	}
	return "private";
}

} // namespace mono
