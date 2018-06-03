#pragma once

#include "mono_config.h"
#include "mono_object.h"

namespace mono
{
class mono_domain;

class mono_string : public mono_object
{
public:
	explicit mono_string(const mono_object& obj);
	explicit mono_string(const mono_domain& domain, const std::string& str);

	auto str() const -> std::string;
};

} // namespace mono
