#pragma once

#include "mono_config.h"

#include <mono/metadata/object.h>

namespace mono
{
class mono_domain;
class mono_class;

class mono_object
{
public:
	explicit mono_object(MonoObject* object);
	explicit mono_object(const mono_domain& domain, const mono_class& cls);

	auto get_class() const -> const mono_class&;

	auto valid() const -> bool;

	auto get_internal_ptr() const -> MonoObject*;

protected:
	std::shared_ptr<mono_class> class_;

	non_owning_ptr<MonoObject> object_ = nullptr;
};

} // namespace mono
