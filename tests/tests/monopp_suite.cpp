#include "monopp_suite.h"

#include "monopp/mono_assembly.h"
#include "monopp/mono_domain.h"
#include "monopp/mono_field.h"
#include "monopp/mono_internal_call.h"
#include "monopp/mono_jit.h"
#include "monopp/mono_method.h"
#include "monopp/mono_method_thunk.h"
#include "monopp/mono_object.h"
#include "monopp/mono_property.h"
#include "monopp/mono_string.h"
#include "monopp/mono_type.h"
#include <iostream>

namespace monopp
{

void MyObject_CreateInternal(const mono::mono_object& this_ptr, float x, const std::string& v)
{
	mono::ignore(this_ptr, x, v);
	std::cout << "FROM C++ : MyObject created." << std::endl;
}

void MyObject_DestroyInternal(const mono::mono_object& this_ptr)
{
	mono::ignore(this_ptr);
	std::cout << "FROM C++ : MyObject deleted." << std::endl;
}

void MyObject_DoStuff(const mono::mono_object& this_ptr, const std::string& value)
{
	mono::ignore(this_ptr, value);
	std::cout << "FROM C++ : DoStuff was called with: " << value << std::endl;
}

std::string MyObject_ReturnAString(const mono::mono_object& this_ptr, const std::string& value)
{
	mono::ignore(this_ptr, value);
	std::cout << "FROM C++ : ReturnAString was called with: " << value << std::endl;
	return "The value: " + value;
}

void test_suite(test::suite& suite)
{
	mono::mono_domain domain("domain");
	mono::mono_domain::set_current_domain(domain);

	// clang-format off
	suite.it("load invalid assembly", [&](test::spec& spec)
    {
		auto expression = [&]()
        {
			// clang-format on
			domain.get_assembly("doesnt_exist_12345.dll");
			// clang-format off
        };
        spec.expect_throw(expression);
    });

    suite.it("load valid assembly", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto refs = assembly.dump_references();
			for(const auto& ref : refs)
			{
				std::cout << ref << std::endl;
			}
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });
    
    suite.it("load valid assembly and bind", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			mono::add_internal_call("Tests.MyObject::CreateInternal", internal_call(MyObject_CreateInternal));
			mono::add_internal_call("Tests.MyObject::DestroyInternal",
									internal_call(MyObject_DestroyInternal));
			mono::add_internal_call("Tests.MyObject::DoStuff", internal_call(MyObject_DoStuff));
			mono::add_internal_call("Tests.MyObject::ReturnAString", internal_call(MyObject_ReturnAString));
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get invalid type", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
            auto assembly = domain.get_assembly("tests_managed.dll");

            spec.expect_throw([&]() 
            { 
                assembly.get_type("SometypeThatDoesntExist12345");
            });
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get monopp valid type", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			assembly.get_type("Tests", "MonoppTest");
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get valid method", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");

			auto method1 = mono::make_thunk<void()>(type, "Method1");
			auto method2 = mono::make_thunk<void(std::string)>(type, "Method2");
			auto method3 = mono::make_thunk<void(int)>(type, "Method3");
			auto method4 = mono::make_thunk<void(int, int)>(type, "Method4");
			auto method5 = mono::make_thunk<std::string(std::string, int)>(type, "Method5");
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get/set field", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");

			auto type = assembly.get_type("Tests", "MonoppTest");

			auto field = type.get_field("someField");

			auto obj = type.new_instance();

			auto someField = field.get_value<int>(obj);
			spec.expect_equals(someField, 12);

			int arg = 6;
			field.set_value(obj, arg);

			someField = field.get_value<int>(obj);
			spec.expect_equals(someField, 6);
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get/set static field", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");

			auto type = assembly.get_type("Tests", "MonoppTest");

			auto field = type.get_field("someFieldStatic");

			auto someField = field.get_value<int>();
			spec.expect_equals(someField, 12);

			int arg = 6;
			field.set_value(arg);

			someField = field.get_value<int>();
			spec.expect_equals(someField, 6);
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get invalid field", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			// clang-format off

            spec.expect_throw([&]()
            { 
                type.get_field("someInvalidField");
            });
        };
        spec.expect_nothrow(expression);			
    });

    suite.it("get/set property", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto prop = type.get_property("someProperty");
			auto obj = type.new_instance();
			spec.expect_true(obj.valid());

			auto someProp = prop.get_value<int>(obj);
			spec.expect_equals(someProp, 12);

			int arg = 55;
			prop.set_value(obj, arg);

			someProp = prop.get_value<int>(obj);
			spec.expect_equals(someProp, 55);
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("get/set static property", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto prop = type.get_property("somePropertyStatic");

			auto someProp = prop.get_value<int>();
			spec.expect_equals(someProp, 6);

			int arg = 55;
			prop.set_value(arg);

			someProp = prop.get_value<int>();
			spec.expect_equals(someProp, 55);
			// clang-format off
        };
        spec.expect_nothrow(expression);			
    });

    suite.it("get invalid property", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
            auto assembly = domain.get_assembly("tests_managed.dll");
            auto type = assembly.get_type("Tests", "MonoppTest");
            spec.expect_throw([&]() 
            { 
                type.get_property("someInvalidProperty");
            });
        };
        spec.expect_nothrow(expression);
    });

    suite.it("call static method 1", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_thunk<int(int)>(type, "Function1");
			const auto number = 1000;
			auto result = method_thunk(number);
			spec.expect_equals(number + 1337, result);
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("call static method 2", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_thunk<void(float, int, float)>(type, "Function2");
			method_thunk(13.37f, 42, 9000.0f);
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("call static method 3", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_thunk<void(std::string)>(type, "Function3");
			method_thunk("Hello!");
			// clang-format off
        };
        spec.expect_nothrow(expression);			
    });
    
    suite.it("call static method 4", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_thunk<std::string(std::string)>(type, "Function4");
			auto expected_string = std::string("Hello!");
			auto result = method_thunk(expected_string);
			spec.expect_equals(result, std::string("The string value was: " + expected_string));
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("call static method 5", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_thunk<void()>(type, "Function5");
			method_thunk();
			// clang-format off
        };
        spec.expect_throw(expression);			
    });

    suite.it("call static method 6", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_thunk<void()>(type, "Function6");
			method_thunk();
			// clang-format off
        };
        spec.expect_nothrow(expression);			
    });

    suite.it("call member method 1", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto obj = type.new_instance();
			auto method_thunk = mono::make_thunk<void()>(type, "Method1");
			method_thunk(obj);
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });

    suite.it("call member method 2", [&](test::spec& spec)
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto obj = type.new_instance();
			auto method_thunk = mono::make_thunk<std::string(std::string, int)>(type, "Method5");
			auto result = method_thunk(obj, "test", 5);
			spec.expect_equals(result, std::string("Return Value: test"));
			// clang-format off
        };
        spec.expect_nothrow(expression);
    });
	// clang-format on
}
}
