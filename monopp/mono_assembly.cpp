#include "mono_assembly.h"
#include "mono_domain.h"
#include "mono_exception.h"

#include "mono_class.h"
#include "mono_class_instance.h"
#include "mono_string.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
namespace mono
{

mono_assembly::mono_assembly(const mono_domain& domain, const std::string& path)
	: domain_(domain)
{
	assembly_ = mono_domain_assembly_open(domain.get_internal_ptr(), path.c_str());

	if(!assembly_)
		throw mono_exception("NATIVE::Could not open assembly with path : " + path);

	image_ = mono_assembly_get_image(assembly_);
}

auto mono_assembly::get_class(const std::string& name) const -> mono_class
{
	return mono_class(*this, name);
}

auto mono_assembly::get_class(const std::string& name_space, const std::string& name) const -> mono_class
{
	return mono_class(*this, name_space, name);
}

auto mono_assembly::new_instance(const mono_class& cls) const -> mono_class_instance
{
	return mono_class_instance(cls);
}

auto mono_assembly::new_string(const std::string& str) const -> mono_string
{
	return mono_string(*this, str);
}

bool mono_assembly::valid() const
{
	return get_internal_ptr() != nullptr;
}

auto mono_assembly::get_internal_ptr() const -> MonoAssembly*
{
	return assembly_;
}
auto mono_assembly::get_image_ptr() const -> MonoImage*
{
	return image_;
}

auto mono_assembly::get_domain() const -> const mono_domain&
{
	return domain_;
}

} // namespace mono
