#pragma once

#include "mono_config.h"

#include <mono/metadata/object.h>

namespace mono
{

class mono_object
{
public:
	explicit mono_object(MonoObject* object);

	auto get_internal_ptr() const -> MonoObject*;

protected:
	non_owning_ptr<MonoObject> object_ = nullptr;
};

} // namespace mono
