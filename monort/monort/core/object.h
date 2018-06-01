#pragma once

#include "monopp/mono_assembly.h"
#include "monopp/mono_field.h"
#include "monopp/mono_gc_handle.h"
#include "monopp/mono_object.h"
#include "monopp/mono_type.h"
#include <memory>

namespace mono
{
namespace managed_interface
{

class object
{
public:
	static void register_internal_calls();
	static void initialize_type_field(const mono_assembly& assembly);

	explicit object(const mono_object& obj);
	virtual ~object();

	object(const object&) noexcept = delete;
	object& operator=(const object&) noexcept = delete;

	static std::unique_ptr<mono_type> object_type;
	static std::unique_ptr<mono_field> native_object_field;

	template <typename T>
	static auto& get_managed_object_as(const mono_object& mono_obj);

private:
	mono_object managed_object_;
	mono_gc_handle gc_handle_;
	mono_scoped_gc_handle gc_scoped_handle_;
};

template <typename T>
auto& object::get_managed_object_as(const mono_object& mono_obj)
{
	return *native_object_field->get_value<T*>(mono_obj);
}

} // namespace managed_interface
} // namespace mono
