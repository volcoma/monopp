#include "mono_method.h"
#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_object.h"

#include <mono/metadata/debug-helpers.h>
namespace mono
{

mono_method::mono_method(const mono_assembly* assembly, MonoClass* cls, MonoObject* object,
						 const std::string& name_with_args)
	: assembly_(assembly)
	, object_(object)
{
	auto desc = mono_method_desc_new((":" + name_with_args).c_str(), 0);
	method_ = mono_method_desc_search_in_class(desc, cls);
	mono_method_desc_free(desc);

	if(!method_)
	{
		std::string cls_name = mono_class_get_name(cls);
		throw mono_exception("NATIVE::Could not get method : " + name_with_args + " for class " + cls_name);
	}
}

mono_method::mono_method(const mono_assembly* assembly, MonoClass* cls, MonoObject* object, const std::string& name,
						 int argc)
	: assembly_(assembly)
	, object_(object)
{
	method_ = mono_class_get_method_from_name(cls, name.c_str(), argc);

	if(!method_)
	{
		std::string cls_name = mono_class_get_name(cls);
		throw mono_exception("NATIVE::Could not get method : " + name + " for class " + cls_name);
	}
}

auto mono_method::get_internal_ptr() const -> MonoMethod*
{
	return method_;
}

mono_method::mono_method(mono_method&&) = default;

auto mono_method::operator=(mono_method &&) -> mono_method& = default;

} // namespace mono
