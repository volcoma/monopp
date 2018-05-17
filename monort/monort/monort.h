#pragma once

#include "core/object.h"
#include "mono_object_wrapper.h"
#include "mono_pod_wrapper.h"

namespace mono
{
namespace managed_interface
{
    inline void init(mono_assembly& core_assembly)
    {
        mono::managed_interface::object::initialize_class_field(core_assembly);
        mono::managed_interface::object::register_internal_calls();        
    }
} // namespace managed_interface

} // namespace mono
