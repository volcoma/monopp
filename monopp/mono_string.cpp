#include "mono_string.h"
#include "mono_assembly.h"
#include "mono_domain.h"
namespace mono
{

mono_string::mono_string(const mono_assembly& assembly, const std::string& str)
	: mono_object(reinterpret_cast<MonoObject*>(
		  mono_string_new(assembly.get_domain().get_internal_ptr(), str.c_str())))
{
}

auto mono_string::str() const -> std::string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	MonoString* mono_str = mono_object_to_string(get_internal_ptr(), nullptr);

	auto raw_utf8_str = mono_string_to_utf8(mono_str);
	std::string str = raw_utf8_str;
	mono_free(raw_utf8_str);
	return str;
}

} // namespace mono
