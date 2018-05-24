#include "mono_object.h"

namespace mono
{

mono_object::mono_object(MonoObject* object)
	: object_(object)
{
}

auto mono_object::get_internal_ptr() const -> MonoObject*
{
	return object_;
}

} // namespace mono
