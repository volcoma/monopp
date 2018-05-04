#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"
#include <mono/jit/jit.h>
#include <string>

namespace mono
{

class mono_class;

class mono_class_property : public common::noncopyable
{
public:
	mono_class_property() = default;
	explicit mono_class_property(const mono_class& monoclass, const std::string& name);

	mono_class_property(mono_class_property&& o);
	auto operator=(mono_class_property&& o) -> mono_class_property&;

	auto get_mono_class_property_ptr() const -> MonoProperty*;

private:
	MonoProperty* property_ = nullptr;
};

} // namespace mono
