#include "mono_class_instance.h"
#include "mono_class.h"
#include "mono_method.h"
#include "mono_domain.h"
#include <cassert>

namespace mono
{

mono_class_instance::mono_class_instance(MonoObject* obj)
	: mono_class_instance(nullptr, obj)
{
}

mono_class_instance::mono_class_instance(mono_assembly* assembly, MonoObject* obj)
	: mono_object(obj)
	, class_(nullptr)
	, assembly_(assembly)
{
}

mono_class_instance::mono_class_instance(mono_assembly* assembly, mono_domain* domain, MonoClass* cls)
	: mono_object(mono_object_new(domain->get_mono_domain_ptr(), cls))
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
	return mono_method(assembly_, get_mono_class_ptr(), object_, name, argc);
}

auto mono_class_instance::get_class() -> mono_class
{
	return mono_class(assembly_, get_mono_class_ptr());
}

auto mono_class_instance::get_mono_class_ptr() -> MonoClass*
{
	assert(object_);

	if(!class_)
		class_ = mono_object_get_class(object_);

	return class_;
}

} // namespace mono
