#include "mono_class_instance.h"
#include "mono_class.h"

namespace mono
{

mono_class_instance::mono_class_instance(const mono_domain& domain, const mono_class& cls)
	: mono_object(mono_object_new(domain.get_internal_ptr(), cls.get_internal_ptr()))
	, class_(cls)
{
	mono_runtime_object_init(object_);
}
auto mono_class_instance::get_class() const -> const mono_class&
{
	return class_;
}
} // namespace mono
