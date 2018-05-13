#pragma once

#include "monopp/mono_assembly.h"
#include "monopp/mono_class.h"
#include "monopp/mono_class_field.h"
#include "monopp/mono_class_instance.h"
#include "monopp/mono_gc_handle.h"
#include "monopp/mono_noncopyable.h"

namespace mono
{
namespace managed_interface
{

class object : public common::noncopyable
{
public:
	static void register_internal_calls();
	static void initialize_class_field(mono_assembly& assembly);

	explicit object(MonoObject* object);
	~object();

	auto get_managed_object() const -> MonoObject*;

	static mono_class object_class;
	static mono_class_field native_object_field;

	template <typename T>
	static auto& get_managed_object_as(MonoObject* this_ptr);

private:
	static void finalize(MonoObject* this_ptr);

	MonoObject* managed_object_;
	mono_gc_handle gc_handle_;
	mono_scoped_gc_handle gc_scoped_handle_;
};

template <typename T>
auto& object::get_managed_object_as(MonoObject* this_ptr)
{
	mono_class_instance cls(this_ptr);
	return *cls.get_field_value<T*>(native_object_field);
}

} // namespace managed_interface
} // namespace mono
