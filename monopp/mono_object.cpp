#include "mono_object.h"
#include "mono_domain.h"

namespace mono
{

mono_object::mono_object()
	: object_(nullptr)
{
}

mono_object::mono_object(MonoObject* object)
	: object_(object)
{
	if(object_)
	{
		type_ = mono_type(mono_object_get_class(object));
	}
}

mono_object::mono_object(const mono_domain& domain, const mono_type& type)
	: type_(type)
	, object_(mono_object_new(domain.get_internal_ptr(), type.get_internal_ptr()))
{
	mono_runtime_object_init(object_);
}
auto mono_object::get_type() const -> const mono_type&
{
	return type_;
}

auto mono_object::valid() const -> bool
{
	return object_ != nullptr;
}

mono_object::operator bool() const
{
	return valid();
}

auto mono_object::get_internal_ptr() const -> MonoObject*
{
	return object_;
}

} // namespace mono
