#include "mono_string.h"
#include "mono_domain.h"

namespace mono
{

mono_string::mono_string(mono_domain* domain, const std::string& str)
	: mono_object(reinterpret_cast<MonoObject*>(mono_string_new(domain->get_internal_ptr(), str.c_str())))
{
}

mono_string::mono_string(mono_string&& o) = default;

auto mono_string::operator=(mono_string&& o) -> mono_string& = default;

auto mono_string::operator=(const std::string& str) -> mono_string&
{
	auto domain = mono_object_get_domain(object_);

	object_ = reinterpret_cast<MonoObject*>(mono_string_new(domain, str.c_str()));
	return *this;
}

auto mono_string::str() const -> std::string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	auto raw_utf8_str = mono_string_to_utf8(get_internal_ptr());
	std::string str = raw_utf8_str;
	mono_free(raw_utf8_str);
	return str;
}

auto mono_string::get_internal_ptr() const -> MonoString*
{
	return mono_object_to_string(object_, nullptr);
}

} // namespace mono
