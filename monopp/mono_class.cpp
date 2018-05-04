#include "mono_class.h"
#include "mono_method.h"
#include "mono_exception.h"
#include "mono_class_field.h"
#include "mono_class_property.h"
#include <cassert>

namespace mono
{

mono_class::mono_class(mono_assembly *assembly, MonoClass *cls)
    : assembly_(assembly)
    , class_(cls)
{
}

mono_class::mono_class(mono_assembly *assembly, MonoImage *image, const std::string &name)
    : mono_class(assembly, image, "", name)
{
}

mono_class::mono_class(mono_assembly *assembly, MonoImage *image, const std::string &name_space,
                       const std::string &name)
    : assembly_(assembly)
{
    class_ = mono_class_from_name(image, name_space.c_str(), name.c_str());

    if (!class_)
        throw mono_exception("Could not get class : " + name_space + "::" + name);
}

mono_class::mono_class(mono_class &&o) = default;

auto mono_class::operator=(mono_class &&o) -> mono_class & = default;

auto mono_class::get_static_function(const std::string &name, int argc /*= 0*/) const -> mono_static_function
{
    assert(class_);
    assert(assembly_);
    return mono_static_function(assembly_, class_, name, argc);
}

auto mono_class::get_mono_class_ptr() const -> MonoClass *
{
    assert(class_);
    return class_;
}

auto mono_class::get_field(const std::string &name) const -> mono_class_field
{
    return mono_class_field(*this, name);
}


auto mono_class::get_property(const std::string &name) const -> mono_class_property
{
    return mono_class_property(*this, name);
}

auto mono_class::get_name() const -> std::string
{
    return mono_class_get_name(get_mono_class_ptr());
}

} // namespace mono
