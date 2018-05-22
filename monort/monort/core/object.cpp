#include "object.h"
#include "monopp/mono_class.h"
#include "monopp/mono_jit.h"

namespace mono
{
namespace managed_interface
{

mono_class object::object_class(nullptr, nullptr);
mono_class_field object::native_object_field;

void object::register_internal_calls()
{
	add_internal_call("Monopp.Core.NativeObject::Finalize", internal_call(object::finalize));
}

void object::initialize_class_field(mono_assembly& assembly)
{
	object_class = assembly.get_class("Monopp.Core", "NativeObject");
	native_object_field = object_class.get_field("nativePtr_");
}

object::~object() = default;

object::object(MonoObject* mono_object)
	: managed_object_(mono_object)
	, gc_handle_(mono_object)
	, gc_scoped_handle_(gc_handle_)
{
	mono_class_instance instance(mono_object);

	assert(mono_class_is_subclass_of(instance.get_class().get_internal_ptr(), object_class.get_internal_ptr(),
									 false) &&
		   "Mono wrapper classes must inherit from Monopp.Core.NativeObject.");

	// Give mono the ownership of the this pointer.
	// When the c# finalize is called then our finalize will
	// delete the pointer
	instance.set_field_value<object*>(native_object_field, this);
}

void object::finalize(MonoObject* this_ptr)
{
	mono_class_instance cls(this_ptr);

	// get the cpp pointer associated with the mono object
	auto this_cpp_ptr = &object::get_managed_object_as<object>(this_ptr);

	// invalidate the mono object's handle to the cpp pointer
	cls.set_field_value(object::native_object_field, nullptr);

	// delete the cpp pointer
	delete this_cpp_ptr;
}

auto object::get_managed_object() const -> MonoObject*
{
	return managed_object_;
}

} // namespace managed_interface
} // namespace mono
