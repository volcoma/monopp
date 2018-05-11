#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include "mono_object.h"

namespace mono
{
class mono_domain;

class mono_string : public mono_object
{
public:
	explicit mono_string(MonoObject* mono_string);
	explicit mono_string(mono_domain* domain, const std::string& str);

	mono_string(mono_string&& o);
	auto operator=(mono_string&& o) -> mono_string&;

	auto operator=(const std::string& str) -> mono_string&;

	auto str() const -> std::string;

	auto get_internal_ptr() const -> MonoString*;
};

} // namespace mono
