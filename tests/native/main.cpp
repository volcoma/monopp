#include "tests.h"
#include "monort/managed_interface/core/object.h"
#include "monort/managed_interface/types_conversion.h"
int main()
{
	if(false == mono::init("tests"))
		return 1;
	{
		mono::mono_domain domain("test");
		tests::test_mono(domain);
	}
//    {
//		mono::mono_domain domain;
//		tests::test_mono(domain);
//	}
    {
        mono::mono_domain domain("test1");
        try
        {
            //https://gist.github.com/bamboo/1207203
            auto& core_assembly = domain.get_assembly("managed_lib.dll");            
            auto& assembly = domain.get_assembly("managed.dll");
            
            auto cls = assembly.get_class("ClassInstanceTest");
            auto obj = assembly.new_class_instance(cls);
            auto prop = cls.get_property("s");
            obj.set_property_value(prop, 12);
            auto s = obj.get_property_value<int>(prop);

            mono::managed_interface::object::initialize_class_field(core_assembly);
            mono::managed_interface::object::register_internal_calls();
//            auto cls = assembly.get_class("Compiler");
//            auto obj = assembly.new_class_instance(cls);
//            auto func = cls.get_static_function_thunk<void()>("Compile");

//            mono::A a;
//            func();
        }
        catch(mono::mono_exception& ex)
        {
            std::cout << ex.what() << std::endl;
            assert(false);
        }
    }

	mono::shutdown();

	return 0;
}
