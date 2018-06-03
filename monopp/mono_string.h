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
	explicit mono_string(const mono_domain& domain, const std::string& as_utf8);

	auto as_utf8() const -> std::string;
	auto as_utf16() const -> std::u16string;
	auto as_utf32() const -> std::u32string;
};

} // namespace mono
