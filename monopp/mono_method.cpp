#include "mono_method.h"
#include "mono_class.h"
#include "mono_exception.h"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>

namespace mono
{

mono_method::mono_method(MonoMethod* method)
{
	method_ = method;
	__generate_meta();
}

mono_method::mono_method(const mono_class& cls, const std::string& name_with_args)
{
	auto desc = mono_method_desc_new((":" + name_with_args).c_str(), 0);
	method_ = mono_method_desc_search_in_class(desc, cls.get_internal_ptr());
	mono_method_desc_free(desc);

	if(!method_)
	{
		const auto& cls_name = cls.get_name();
		throw mono_exception("NATIVE::Could not get method : " + name_with_args + " for class " + cls_name);
	}
	__generate_meta();
}

mono_method::mono_method(const mono_class& cls, const std::string& name, int argc)
{
	method_ = mono_class_get_method_from_name(cls.get_internal_ptr(), name.c_str(), argc);

	if(!method_)
	{
		const auto& cls_name = cls.get_name();
		throw mono_exception("NATIVE::Could not get method : " + name + " for class " + cls_name);
	}
	__generate_meta();
}

auto mono_method::get_internal_ptr() const -> MonoMethod*
{
	return method_;
}

void mono_method::__generate_meta()
{
	signature_ = mono_method_signature(method_);
	name_ = mono_method_get_name(method_);
	fullname_ = mono_method_full_name(method_, true);
	std::string storage = (is_static() ? " static " : " ");
	full_declname_ = to_string(get_visibility()) + storage + fullname_;
}

auto mono_method::get_return_type() const -> mono_type
{
	return mono_type(mono_signature_get_return_type(signature_));
}

auto mono_method::get_name() const -> const std::string&
{
	return name_;
}

auto mono_method::get_fullname() const -> const std::string&
{
	return fullname_;
}
auto mono_method::get_full_declname() const -> const std::string&
{
	return full_declname_;
}
auto mono_method::get_visibility() const -> visibility
{

	uint32_t flags = mono_method_get_flags(method_, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

	if(flags == MONO_METHOD_ATTR_PRIVATE)
		return visibility::private_;
	else if(flags == MONO_METHOD_ATTR_FAM_AND_ASSEM)
		return visibility::protected_internal_;
	else if(flags == MONO_METHOD_ATTR_ASSEM)
		return visibility::internal_;
	else if(flags == MONO_METHOD_ATTR_FAMILY)
		return visibility::protected_;
	else if(flags == MONO_METHOD_ATTR_PUBLIC)
		return visibility::public_;

	assert(false);

	return visibility::private_;
}

auto mono_method::is_static() const -> bool
{
	return !mono_signature_is_instance(signature_);
}

} // namespace mono
