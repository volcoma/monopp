#include "mono_method.h"
#include "mono_class.h"
#include "mono_exception.h"

#include <mono/metadata/debug-helpers.h>
namespace mono
{

mono_method::mono_method(const mono_class& cls, const std::string& name_with_args)
	: class_(cls)
{
	auto desc = mono_method_desc_new((":" + name_with_args).c_str(), 0);
	method_ = mono_method_desc_search_in_class(desc, cls.get_internal_ptr());
	mono_method_desc_free(desc);

	if(!method_)
	{
		std::string cls_name = mono_class_get_name(cls.get_internal_ptr());
		throw mono_exception("NATIVE::Could not get method : " + name_with_args + " for class " + cls_name);
	}
}

mono_method::mono_method(const mono_class& cls, const std::string& name, int argc)
	: class_(cls)
{
	method_ = mono_class_get_method_from_name(cls.get_internal_ptr(), name.c_str(), argc);

	if(!method_)
	{
		std::string cls_name = mono_class_get_name(cls.get_internal_ptr());
		throw mono_exception("NATIVE::Could not get method : " + name + " for class " + cls_name);
	}
}

auto mono_method::get_internal_ptr() const -> MonoMethod*
{
	return method_;
}

auto mono_method::get_class() const -> const mono_class&
{
	return class_;
}

auto mono_method::get_assembly() const -> const mono_assembly&
{
	return class_.get_assembly();
}

} // namespace mono
