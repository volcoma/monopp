#pragma once

#include "core/object.h"
#include <memory>

namespace mono
{
namespace managed_interface
{

#define register_basic_mono_converter_for_wrapper(type)                                                      \
	template <>                                                                                              \
	struct convert_mono_type<type>                                                                           \
	{                                                                                                        \
		using mono_type_name = MonoObject*;                                                                  \
                                                                                                             \
		static auto to_mono(mono_assembly& assembly, type wrapper) -> mono_type_name                            \
		{                                                                                                    \
			return managed_interface::mono_object_wrapper<type>::create(assembly, wrapper);                            \
		}                                                                                                    \
                                                                                                             \
		static auto from_mono(mono_type_name object) -> type                                                    \
		{                                                                                                    \
			return managed_interface::mono_object_wrapper<type>::get_native_object(object);                  \
		}                                                                                                    \
	}

template <typename T>
class mono_object_wrapper : public object
{
public:
	explicit mono_object_wrapper(MonoObject* mono_object, T obj);
	~mono_object_wrapper();

	/*!
	 * Create a new MonoObject and associate this wrapper to it.
	 */
	static auto create(mono_assembly& assembly, T obj) -> MonoObject*;

	/*!
	 * Create a wrapper for an existing MonoObject.
	 */
	static void create(MonoObject* mono_object, T obj);

	static auto& get_native_object(MonoObject* mono_object);

private:
	T native_object;
};

template <typename T>
auto mono_object_wrapper<T>::create(mono_assembly& assembly, T obj) -> MonoObject*
{
	auto instance = assembly.new_class_instance(object::object_class).get_mono_object();
	create(instance, std::move(obj));
	return instance;
}

template <typename T>
void mono_object_wrapper<T>::create(MonoObject* mono_object, T obj)
{
	std::make_unique<mono_object_wrapper<T>>(mono_object, std::move(obj)).release();
}

template <typename T>
mono_object_wrapper<T>::mono_object_wrapper(MonoObject* mono_object, T obj)
	: object(mono_object)
	, native_object(std::move(obj))
{
}

template <typename T>
mono_object_wrapper<T>::~mono_object_wrapper() = default;

template <typename T>
auto& mono_object_wrapper<T>::get_native_object(MonoObject* mono_object)
{
	return object::get_managed_object_as<mono_object_wrapper<T>>(mono_object).native_object;
}

} // namespace managed_interface
} // namespace mono
