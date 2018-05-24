#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include "mono_object.h"

namespace mono
{
class mono_assembly;

class mono_string : public mono_object
{
public:
	using mono_object::mono_object;
	explicit mono_string(const mono_assembly& assembly, const std::string& str);

	auto str() const -> std::string;
};

} // namespace mono
