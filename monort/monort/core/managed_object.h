#pragma once

#include "monopp/mono_assembly.h"
#include "monopp/mono_field_invoker.h"
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

	explicit object(mono_object obj);
	virtual ~object();

	object(const object&) noexcept = delete;
	object& operator=(const object&) noexcept = delete;

	static std::unique_ptr<mono_type>& get_object_type();
	static std::unique_ptr<mono_field>& get_native_object_field();

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
	const auto& field = *get_native_object_field();
	auto mutable_field = make_field_invoker<T*>(field);
	return *mutable_field.get_value(mono_obj);
}

} // namespace managed_interface
} // namespace mono
