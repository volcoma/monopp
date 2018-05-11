#include "mono_assembly.h"
#include "mono_class.h"
#include "mono_class_instance.h"
#include "mono_domain.h"
#include "mono_exception.h"
#include "mono_string.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
namespace mono
{

mono_assembly::mono_assembly(mono_domain* domain, const std::string& path)
	: domain_(domain)
{
	assembly_ = mono_domain_assembly_open(domain_->get_internal_ptr(), path.c_str());

	if(!assembly_)
		throw mono_exception("NATIVE::Could not open assembly with path : " + path);

	image_ = mono_assembly_get_image(assembly_);
}

mono_assembly::mono_assembly(const mono_assembly& o) = default;
auto mono_assembly::operator=(const mono_assembly& o) -> mono_assembly& = default;

mono_assembly::mono_assembly(mono_assembly&& o) = default;

auto mono_assembly::operator=(mono_assembly&& o) -> mono_assembly& = default;

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
	return mono_class_instance(this, domain_, cls.get_internal_ptr());
}

auto mono_assembly::new_string(const std::string& str) const -> mono_string
{
	return mono_string(domain_, str);
}

auto mono_assembly::valid() const -> bool
{
	return domain_ != nullptr && assembly_ != nullptr && image_ != nullptr;
}

auto mono_assembly::get_domain() const -> mono_domain*
{
	return domain_;
}

} // namespace mono
