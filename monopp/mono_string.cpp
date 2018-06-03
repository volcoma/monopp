#include "mono_string.h"
#include "mono_domain.h"

namespace mono
{

mono_string::mono_string(const mono_object& obj)
	: mono_object(obj)
{
}

mono_string::mono_string(const mono_domain& domain, const std::string& str)
	: mono_object(reinterpret_cast<MonoObject*>(mono_string_new(domain.get_internal_ptr(), str.c_str())))
{
}

auto mono_string::as_utf8() const -> std::string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	MonoString* mono_str = mono_object_to_string(get_internal_ptr(), nullptr);

	auto raw_str = mono_string_to_utf8(mono_str);
	std::string str = reinterpret_cast<std::string::value_type*>(raw_str);
	mono_free(raw_str);
	return str;
}

auto mono_string::as_utf16() const -> std::u16string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	MonoString* mono_str = mono_object_to_string(get_internal_ptr(), nullptr);

	auto raw_str = mono_string_to_utf16(mono_str);
	std::u16string str = reinterpret_cast<std::u16string::value_type*>(raw_str);
	mono_free(raw_str);
	return str;
}

auto mono_string::as_utf32() const -> std::u32string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	MonoString* mono_str = mono_object_to_string(get_internal_ptr(), nullptr);

	auto raw_str = mono_string_to_utf32(mono_str);
	std::u32string str = reinterpret_cast<std::u32string::value_type*>(raw_str);
	mono_free(raw_str);
	return str;
}

} // namespace mono
