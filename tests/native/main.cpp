#include "tests.h"
#include "monort/managed_interface/core/object.h"
int main()
{
	if(false == mono::init("tests"))
		return 1;
	{
		mono::mono_domain domain("test");
		
		auto& core_assembly = domain.get_assembly("managed_lib.dll");            
        auto& assembly = domain.get_assembly("managed.dll");
		mono::managed_interface::object::initialize_class_field(core_assembly);
        mono::managed_interface::object::register_internal_calls();
		tests::test_mono(domain);
	}


	mono::shutdown();

	return 0;
}
