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
{
	assembly_ = mono_domain_assembly_open(domain.get_internal_ptr(), path.c_str());

	if(!assembly_)
		throw mono_exception("NATIVE::Could not open assembly with path : " + path);

	image_ = mono_assembly_get_image(assembly_);
}

auto mono_assembly::get_class(const std::string& name) const -> mono_class
{
	return mono_class(image_, name);
}

auto mono_assembly::get_class(const std::string& name_space, const std::string& name) const -> mono_class
{
	return mono_class(image_, name_space, name);
}

bool mono_assembly::valid() const
{
	return get_internal_ptr() != nullptr;
}

auto mono_assembly::get_internal_ptr() const -> MonoAssembly*
{
	return assembly_;
}

} // namespace mono
