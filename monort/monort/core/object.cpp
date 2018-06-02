#include "object.h"
#include "monopp/mono_internal_call.h"
#include "monopp/mono_jit.h"
#include "monopp/mono_type.h"

namespace mono
{
namespace managed_interface
{

std::unique_ptr<mono_type> object::object_type;
std::unique_ptr<mono_field> object::native_object_field;

void finalize(const mono::mono_object& obj)
{
	// get the cpp pointer associated with the mono object
	auto this_cpp_ptr = &object::get_managed_object_as<object>(obj);

	// invalidate the mono object's handle to the cpp pointer
	object::native_object_field->set_value(obj, nullptr);

	// delete the cpp pointer
	delete this_cpp_ptr;
}
void object::register_internal_calls()
{
	add_internal_call("Monopp.Core.NativeObject::Finalize", internal_call(finalize));
}

void object::initialize_type_field(const mono_assembly& assembly)
{
	auto type = assembly.get_type("Monopp.Core", "NativeObject");
	object_type = std::make_unique<mono_type>(std::move(type));
	auto field = object_type->get_field("native_this_");
	native_object_field = std::make_unique<mono_field>(std::move(field));
}

object::~object() = default;

object::object(mono_object obj)
	: managed_object_(std::move(obj))
	, gc_handle_(managed_object_.get_internal_ptr())
	, gc_scoped_handle_(gc_handle_)
{
	assert(managed_object_.get_type().is_derived_from(*object_type) &&
		   "Mono wrapper classes must inherit from Monopp.Core.NativeObject.");

	// Give mono the ownership of the this pointer.
	// When the c# finalize is called then our finalize will
	// delete the pointer
	native_object_field->set_value<object*>(managed_object_, this);
}

} // namespace managed_interface
} // namespace mono
