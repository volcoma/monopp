#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"

#include <mono/metadata/class.h>

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

	auto get_internal_ptr() const -> MonoProperty*;
	auto get_name() const -> std::string;

private:
	MonoProperty* property_ = nullptr;
	std::string name_;
};

} // namespace mono
