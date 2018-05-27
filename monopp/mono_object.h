#pragma once

#include "mono_config.h"

#include <mono/metadata/object.h>

namespace mono
{
class mono_domain;
class mono_type;

class mono_object
{
public:
	explicit mono_object(MonoObject* object);
	explicit mono_object(const mono_domain& domain, const mono_type& type);

	auto get_type() const -> const mono_type&;

	auto valid() const -> bool;

	auto get_internal_ptr() const -> MonoObject*;

protected:
	std::shared_ptr<mono_type> type_;

	non_owning_ptr<MonoObject> object_ = nullptr;
};

} // namespace mono
