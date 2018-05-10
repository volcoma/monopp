#include "mono_class_instance.h"
#include "mono_class.h"
#include "mono_method.h"
#include <cassert>

namespace mono
{

mono_class_instance::mono_class_instance(MonoObject* obj)
	: mono_object(obj)
	, class_(mono_object_get_class(obj))
{
}

mono_class_instance::mono_class_instance(mono_assembly* assembly, mono_domain* domain, MonoClass* cls)
	: mono_object(mono_object_new(domain->get_internal_ptr(), cls))
	, class_(cls)
	, assembly_(assembly)
{
	mono_runtime_object_init(object_);
}

mono_class_instance::mono_class_instance(mono_class_instance&& o) = default;

auto mono_class_instance::operator=(mono_class_instance&& o) -> mono_class_instance& = default;

auto mono_class_instance::get_method(const std::string& name, int argc /*= 0*/) -> mono_method
{
	assert(assembly_);
	assert(object_);
	return mono_method(assembly_, class_, object_, name, argc);
}

auto mono_class_instance::get_class() -> mono_class
{
	return mono_class(assembly_, class_);
}

} // namespace mono
