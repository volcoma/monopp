#include "mono_assembly.h"
#include "mono_class.h"
#include "mono_class_instance.h"
#include "mono_domain.h"
#include "mono_exception.h"
#include "mono_string.h"
#include <mono/metadata/assembly.h>

namespace mono
{

mono_assembly::mono_assembly(mono_domain* domain, MonoAssembly* assembly)
	: domain_(domain)
	, assembly_(assembly)
	, image_(mono_assembly_get_image(assembly_))
{
}

mono_assembly::mono_assembly(mono_domain* domain, const std::string& path)
	: domain_(domain)
{
	assembly_ = mono_domain_assembly_open(domain->get_mono_domain_ptr(), path.c_str());

	if(!assembly_)
		throw mono_exception("Could not open assembly with path : " + path);

	image_ = mono_assembly_get_image(assembly_);
}

mono_assembly::mono_assembly(mono_assembly&& o) = default;

auto mono_assembly::operator=(mono_assembly&& o) -> mono_assembly& = default;

auto mono_assembly::get_mono_assembly_ptr() const -> MonoAssembly*
{
	return assembly_;
}

auto mono_assembly::get_mono_domain_ptr() const -> mono_domain*
{
	return domain_;
}

auto mono_assembly::get_class(const std::string& name) -> mono_class
{
	return mono_class(this, image_, name);
}

auto mono_assembly::get_class(const std::string& name_space, const std::string& name) -> mono_class
{
	return mono_class(this, image_, name_space, name);
}

auto mono_assembly::new_class_instance(const mono_class& cls) -> mono_class_instance
{
	return mono_class_instance(this, domain_, cls.get_mono_class_ptr());
}

auto mono_assembly::new_string(const std::string& str) const -> mono_string
{
	return mono_string(domain_, str);
}

auto mono_assembly::valid() const -> bool
{
	return domain_ != nullptr && assembly_ != nullptr && image_ != nullptr;
}

} // namespace mono
