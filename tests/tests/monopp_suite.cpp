#include "monopp_suite.h"

#include "monopp/mono_assembly.h"
#include "monopp/mono_domain.h"
#include "monopp/mono_field_invoker.h"
#include "monopp/mono_internal_call.h"
#include "monopp/mono_method_invoker.h"
#include "monopp/mono_object.h"
#include "monopp/mono_property_invoker.h"
#include "monopp/mono_string.h"
#include "monopp/mono_type.h"
#include "suite/suite.hpp"
#include <chrono>
#include <iostream>
#include <thread>
namespace monopp
{

void MyObject_CreateInternal(const mono::mono_object& this_ptr, float x, const std::string& v)
{
	mono::ignore(this_ptr, x, v);
	// std::cout << "FROM C++ : MyObject created." << std::endl;
}

void MyObject_DestroyInternal(const mono::mono_object& this_ptr)
{
	mono::ignore(this_ptr);
	// std::cout << "FROM C++ : MyObject deleted." << std::endl;
}

void MyObject_DoStuff(const mono::mono_object& this_ptr, const std::string& value)
{
	mono::ignore(this_ptr, value);
	// std::cout << "FROM C++ : DoStuff was called with: " << value << std::endl;
}

std::string MyObject_ReturnAString(const mono::mono_object& this_ptr, const std::string& value)
{
	mono::ignore(this_ptr, value);
	// std::cout << "FROM C++ : ReturnAString was called with: " << value << std::endl;
	return "The value: " + value;
}

void test_suite()
{
	mono::mono_domain domain("domain");
	mono::mono_domain::set_current_domain(domain);

	// clang-format off
    suite::test("load invalid assembly", [&]()
    {
        EXPECT(THROWS_AS(domain.get_assembly("doesnt_exist_12345.dll"), mono::mono_exception));
    });

    suite::test("load valid assembly", [&]()
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
        EXPECT(NOTHROWS(expression()));
    });
    
    suite::test("load valid assembly and bind", [&]()
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
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get invalid type", [&]()
    {
        auto expression = [&]()
        {
            auto assembly = domain.get_assembly("tests_managed.dll");

            EXPECT(THROWS(assembly.get_type("SometypeThatDoesntExist12345")));
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get monopp valid type", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			assembly.get_type("Tests", "MonoppTest");
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get valid method", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");

			auto method1 = mono::make_method_invoker<void()>(type, "Method1");

			auto method2 = mono::make_method_invoker<void(std::string)>(type, "Method2");

			auto method3 = mono::make_method_invoker<void(int)>(type, "Method3");

			auto method4 = mono::make_method_invoker<void(int, int)>(type, "Method4");

			auto method5 = mono::make_method_invoker<std::string(std::string, int)>(type, "Method5");

			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get/set field", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");

			auto type = assembly.get_type("Tests", "MonoppTest");

			auto field = type.get_field("someField");

			auto mutable_field = mono::make_field_invoker<int>(field);

			auto obj = type.new_instance();

			auto someField = mutable_field.get_value(obj);

			EXPECT(someField == 12);

			int arg = 6;
			mutable_field.set_value(obj, arg);

			someField = mutable_field.get_value(obj);
			EXPECT(someField == 6);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get/set static field", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");

			auto field = type.get_field("someFieldStatic");

			auto mutable_field = mono::make_field_invoker<int>(field);

			auto someField = mutable_field.get_value();

			EXPECT(someField == 12);

			int arg = 6;
			mutable_field.set_value(arg);

			someField = mutable_field.get_value();
			EXPECT(someField == 6);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get invalid field", [&]()
    {
        auto expression = [&]()
        {
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
            EXPECT(THROWS(type.get_field("someInvalidField")));
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get/set property", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");

			auto prop = type.get_property("someProperty");

			auto mutable_prop = mono::make_property_invoker<int>(prop);

			auto obj = type.new_instance();

			EXPECT(obj.valid());

			auto someProp = mutable_prop.get_value(obj);

			EXPECT(someProp == 12);

			int arg = 55;
			mutable_prop.set_value(obj, arg);

			someProp = mutable_prop.get_value(obj);
			EXPECT(someProp == 55);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get/set static property", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto prop = type.get_property("somePropertyStatic");
			auto mutable_prop = mono::make_property_invoker<int>(prop);
			auto someProp = mutable_prop.get_value();
			EXPECT(someProp == 6);

			int arg = 55;
			mutable_prop.set_value(arg);

			someProp = mutable_prop.get_value();
			EXPECT(someProp == 55);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("get invalid property", [&]()
    {
        auto expression = [&]()
        {
            auto assembly = domain.get_assembly("tests_managed.dll");
            auto type = assembly.get_type("Tests", "MonoppTest");
            EXPECT(THROWS(type.get_property("someInvalidProperty")));
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call static method 1", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_method_invoker<int(int)>(type, "Function1");
			const auto number = 1000;
			auto result = method_thunk(number);
			EXPECT(number + 1337 == result);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call static method 2", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_method_invoker<void(float, int, float)>(type, "Function2");
			method_thunk(13.37f, 42, 9000.0f);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call static method 3", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_method_invoker<void(std::string)>(type, "Function3");
			method_thunk("Hello!");
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });
    
    suite::test("call static method 4", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_method_invoker<std::string(std::string)>(type, "Function4");
			auto expected_string = std::string("Hello!");
			auto result = method_thunk(expected_string);
			EXPECT(result == std::string("The string value was: " + expected_string));
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call static method 5", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_method_invoker<void()>(type, "Function5");
			EXPECT(THROWS(method_thunk()));
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call static method 6", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto method_thunk = mono::make_method_invoker<void()>(type, "Function6");
			method_thunk();
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call member method 1", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto obj = type.new_instance();
			auto method_thunk = mono::make_method_invoker<void()>(type, "Method1");
			method_thunk(obj);
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });

    suite::test("call member method 2", [&]()
    {
        auto expression = [&]()
        {
			// clang-format on
			auto assembly = domain.get_assembly("tests_managed.dll");
			auto type = assembly.get_type("Tests", "MonoppTest");
			auto obj = type.new_instance();
			auto method_thunk = mono::make_method_invoker<std::string(std::string, int)>(type, "Method5");
			auto result = method_thunk(obj, "test", 5);
			EXPECT(result == std::string("Return Value: test"));
			// clang-format off
        };
        EXPECT(NOTHROWS(expression()));
    });
	// clang-format on
}
}
