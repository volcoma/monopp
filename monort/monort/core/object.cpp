#include "object.h"
#include "monopp/mono_type.h"
#include "monopp/mono_internal_call.h"
#include "monopp/mono_jit.h"

namespace mono
{
namespace managed_interface
{

std::unique_ptr<mono_type> object::object_type;
std::unique_ptr<mono_field> object::native_object_field;

void object::register_internal_calls()
{
	add_internal_call("Monopp.Core.NativeObject::Finalize", internal_call(object::finalize));
}

void object::initialize_type_field(const mono_assembly& assembly)
{
	auto type = assembly.get_type("Monopp.Core", "NativeObject");
	object_type = std::make_unique<mono_type>(std::move(type));
	auto field = object_type->get_field("native_this_");
	native_object_field = std::make_unique<mono_field>(std::move(field));
}

object::~object() = default;

object::object(MonoObject* obj)
	: managed_object_(obj)
	, gc_handle_(obj)
	, gc_scoped_handle_(gc_handle_)
{
	const auto& type = managed_object_.get_type();
	assert(type.is_derived_from(*object_type) &&
		   "Mono wrapper classes must inherit from Monopp.Core.NativeObject.");

	// Give mono the ownership of the this pointer.
	// When the c# finalize is called then our finalize will
	// delete the pointer
	native_object_field->set_value<object*>(managed_object_, this);
}

void object::finalize(MonoObject* this_ptr)
{
	mono_object obj(this_ptr);

	// get the cpp pointer associated with the mono object
	auto this_cpp_ptr = &get_managed_object_as<object>(this_ptr);

	// invalidate the mono object's handle to the cpp pointer
	native_object_field->set_value(obj, nullptr);

	// delete the cpp pointer
	delete this_cpp_ptr;
}

} // namespace managed_interface
} // namespace mono
