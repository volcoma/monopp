#include "mono_object.h"
#include "mono_class.h"
#include "mono_domain.h"

namespace mono
{

mono_object::mono_object(MonoObject* object)
	: object_(object)
{
	class_ = std::make_shared<mono_class>(mono_object_get_class(object_));
}

mono_object::mono_object(const mono_domain& domain, const mono_class& cls)
	: class_(std::make_shared<mono_class>(cls))
	, object_(mono_object_new(domain.get_internal_ptr(), cls.get_internal_ptr()))
{
	mono_runtime_object_init(object_);
}
auto mono_object::get_class() const -> const mono_class&
{
	return *class_;
}

auto mono_object::valid() const -> bool
{
	return object_ != nullptr;
}

auto mono_object::get_internal_ptr() const -> MonoObject*
{
	return object_;
}

} // namespace mono
