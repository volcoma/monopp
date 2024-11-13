#pragma once

#include "mono_config.h"
#include "mono_domain.h"
#include "mono_object.h"
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace mono
{

class mono_array_base : public mono_object
{
public:
	//using mono_object::mono_object;
	// Construct from an existing MonoArray
	explicit mono_array_base(MonoArray* arr)
		: mono_object(reinterpret_cast<MonoObject*>(arr))
	{
	}

	explicit mono_array_base(const mono_object& obj)
		: mono_object(obj)
	{
	}

	// Special constructor for std::vector<mono_object>
	mono_array_base(const mono_domain& domain, const std::vector<mono_object>& vec)
		: mono_object(reinterpret_cast<MonoObject*>(
			  mono_array_new(domain.get_internal_ptr(),
							 vec.empty() ? nullptr : vec[0].get_type().get_internal_ptr(), vec.size())))
	{
		if(!vec.empty())
		{
			for(size_t i = 0; i < vec.size(); ++i)
			{
				set(i, vec[i]);
			}
		}
	}

	// Get the size of the array
	auto size() const -> size_t
	{
		return mono_array_length(get_internal_array());
	}

	// Overloaded get method for mono_object
	auto get_object(size_t index) const -> mono_object
	{
		MonoObject* obj = mono_array_get(get_internal_array(), MonoObject*, index);
		return mono_object(obj);
	}

	// Overloaded set method for mono_object
	void set(size_t index, const mono_object& value)
	{
		mono_array_set(get_internal_array(), MonoObject*, index, value.get_internal_ptr());
	}

	// Convert to std::vector<mono_object>
	auto to_object_vector() const -> std::vector<mono_object>
	{
		std::vector<mono_object> vec(size());
		for(size_t i = 0; i < size(); ++i)
		{
			vec[i] = get_object(i);
		}
		return vec;
	}

protected:
	auto get_internal_array() const -> MonoArray*
	{
		return reinterpret_cast<MonoArray*>(object_);
	}
};

template <typename T>
class mono_array : public mono_array_base
{
public:
	using mono_array_base::mono_array_base;

	// Construct a new MonoArray from a std::vector of primitive types
	mono_array(const mono_domain& domain, const std::vector<T>& vec)
		: mono_array_base(mono_array_new(domain.get_internal_ptr(), mono_class_from_type(), vec.size()))
	{
		for(size_t i = 0; i < vec.size(); ++i)
		{
			set(i, vec[i]);
		}
	}

	// Special constructor for std::vector<mono_object>
	mono_array(const mono_domain& domain, const std::vector<mono_object>& vec)
		: mono_array_base(mono_array_new(domain.get_internal_ptr(),
										 vec.empty() ? nullptr : vec[0].get_type().get_internal_ptr(),
										 vec.size()))
	{
		if(!vec.empty())
		{
			for(size_t i = 0; i < vec.size(); ++i)
			{
				set(i, vec[i]);
			}
		}
	}

	// Access element at index for primitive types
	auto get(size_t index) const -> T
	{
		return mono_array_get(get_internal_array(), T, index);
	}

	// Set element at index for primitive types
	void set(size_t index, const T& value)
	{
		mono_array_set(get_internal_array(), T, index, value);
	}

	// Convert to std::vector for primitive types
	auto to_vector() const -> std::vector<T>
	{
		std::vector<T> vec(size());
		for(size_t i = 0; i < size(); ++i)
		{
			vec[i] = get(i);
		}
		return vec;
	}

private:
	// Helper to get MonoClass for T without if constexpr
	static MonoClass* mono_class_from_type()
	{
		MonoClass* mono_type = nullptr;
		if(std::is_same<T, int>::value)
		{
			mono_type = mono_get_int32_class();
		}
		else if(std::is_same<T, float>::value)
		{
			mono_type = mono_get_single_class();
		}
		else if(std::is_same<T, double>::value)
		{
			mono_type = mono_get_double_class();
		}
		else if(std::is_same<T, bool>::value)
		{
			mono_type = mono_get_boolean_class();
		}
		else if(std::is_same<T, char>::value)
		{
			mono_type = mono_get_char_class();
		}
		else if(std::is_same<T, mono_object>::value)
		{
			throw std::runtime_error("Use the mono_object-specific constructor for mono_array.");
		}
		else
		{
			throw std::runtime_error("Unsupported type for mono_array");
		}
		return mono_type;
	}
};

} // namespace mono
