#pragma once

#include "mono_config.h"
#include "mono_type.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/object.h>
END_MONO_INCLUDE

namespace mono
{
class mono_domain;

class mono_object
{
public:
	explicit mono_object(MonoObject* object);
	explicit mono_object(const mono_domain& domain, const mono_type& type);

	auto get_type() const -> const mono_type&;

	auto valid() const -> bool;

	auto get_internal_ptr() const -> MonoObject*;

protected:
	mono_type type_;

	non_owning_ptr<MonoObject> object_ = nullptr;
};

} // namespace mono
