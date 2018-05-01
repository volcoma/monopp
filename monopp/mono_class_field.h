#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include <mono/jit/jit.h>
#include <string>

namespace mono
{

class mono_class;

class mono_class_field : public common::noncopyable
{
public:
	mono_class_field() = default;
	explicit mono_class_field(const mono_class& monoclass, const std::string& name);

	mono_class_field(mono_class_field&& o);
	auto operator=(mono_class_field&& o) -> mono_class_field&;

	auto get_mono_class_field_ptr() const -> MonoClassField*;

private:
	MonoClassField* field_ = nullptr;
};

} // namespace mono
