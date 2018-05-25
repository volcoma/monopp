#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_object.h"

namespace mono
{

class mono_domain;
class mono_class;

class mono_class_instance : public mono_object
{
public:
	explicit mono_class_instance(const mono_domain& domain, const mono_class& cls);
};

} // namespace mono
