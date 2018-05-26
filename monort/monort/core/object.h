#pragma once

#include "monopp/mono_assembly.h"
#include "monopp/mono_class.h"
#include "monopp/mono_class_field.h"
#include "monopp/mono_class_instance.h"
#include "monopp/mono_gc_handle.h"
#include "monopp/mono_noncopyable.h"
#include <memory>

namespace mono
{
namespace managed_interface
{

class object : public common::noncopyable
{
public:
	static void register_internal_calls();
	static void initialize_class_field(const mono_assembly& assembly);

	explicit object(MonoObject* object);
	virtual ~object();

	static std::unique_ptr<mono_class> object_class;
	static std::unique_ptr<mono_class_field> native_object_field;

	template <typename T>
	static auto& get_managed_object_as(MonoObject* this_ptr);

private:
	static void finalize(MonoObject* this_ptr);

	mono_object managed_object_;
	mono_gc_handle gc_handle_;
	mono_scoped_gc_handle gc_scoped_handle_;
};

template <typename T>
auto& object::get_managed_object_as(MonoObject* this_ptr)
{
	mono::mono_object obj(this_ptr);
	return *native_object_field->get_value<T*>(obj);
}

} // namespace managed_interface
} // namespace mono
