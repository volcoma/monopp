#include "tests.h"
#include "mono_assembly.h"
#include "mono_class.h"
#include "mono_class_instance.h"
#include "mono_method.h"
#include "mono_string.h"

namespace tests
{
struct log
{
    log(const char* f)
    {
        separator();
        std::cout << f << std::endl;
    }
    
    ~log()
    {
        separator();
    }
    
    void separator()
    {
        std::cout << "-------" << std::endl;        
    }
};

void test_mono_jit_get_assembly_fail(mono::mono_domain& domain)
{
    log l(__func__);

	try
	{
		domain.get_assembly("some_assembly_that_doesnt_exist_12345.dll");
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void test_mono_jit_get_assembly(mono::mono_domain& domain)
{
    log l(__func__);    
	try
	{
		domain.get_assembly("managed.dll");
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		assert(false);
	}
}

void test_mono_jit_get_class_fail(mono::mono_domain& domain)
{
    log l(__func__);    
	try
	{
		auto& assembly = domain.get_assembly("managed.dll");
		try
		{
			auto cls = assembly.get_class("SomeClassThatDoesntExist12345");
		}
		catch(mono::mono_exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		assert(false);
	}
}

void test_mono_jit_get_class(mono::mono_domain& domain)
{
    log l(__func__);    
	try
	{
		auto& assembly = domain.get_assembly("managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		assert(false);
	}
}

void test_mono_jit_get_method(mono::mono_domain& domain)
{
    log l(__func__);    
	try
	{
		auto& assembly = domain.get_assembly("managed.dll");

		auto cls = assembly.get_class("ClassInstanceTest");

		auto obj = assembly.new_class_instance(cls);

		auto method_Method = obj.get_method("Method");
		auto method_MethodWithParameter = obj.get_method("MethodWithParameter", 1);
		auto method_MethodWithParameterAndReturnValue =
			obj.get_method("MethodWithParameterAndReturnValue", 1);
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		assert(false);
	}
}


void test_mono_jit_get_field(mono::mono_domain &domain)
{
    log l(__func__);    
	try
	{
		auto& assembly = domain.get_assembly("managed.dll");

		auto cls = assembly.get_class("ClassInstanceTest");

        auto field = cls.get_field("someField");
		auto obj = assembly.new_class_instance(cls);

		auto someField = obj.get_field_value<int>(field);
        int a = 0;
        a++;
        (void)someField;
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}


void test_mono_jit_get_field_fail(mono::mono_domain &domain)
{
    log l(__func__);    
	try
	{
		auto& assembly = domain.get_assembly("managed.dll");

		auto cls = assembly.get_class("ClassInstanceTest");

        auto field = cls.get_field("someField1");
		auto obj = assembly.new_class_instance(cls);

		auto someField = obj.get_field_value<int>(field);
        int a = 0;
        a++;
        (void)someField;
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}


void test_mono_call_thunk(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_function_thunk<int(int)>("FunctionWithIntParam");
	const auto number = 1000;
	auto result = method_thunk(number);
	assert(number + 1337 == result);
}

void test_mono_call_thunk2(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_function_thunk<void(float, int, float)>("VoidFunction");
	method_thunk(13.37f, 42, 9000.0f);
}

void test_mono_call_thunk3(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_function_thunk<void(std::string)>("FunctionWithStringParam");
	method_thunk("Hello!");
}

void test_mono_call_thunk4(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_function_thunk<std::string(std::string)>("StringReturnFunction");
	auto expected_string = std::string("Hello!");
	auto result = method_thunk(expected_string);
	assert(result == std::string("The string value was: " + expected_string));
}

void test_mono_call_thunk_with_exception(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_function_thunk<void()>("ExceptionFunction");
	try
	{
		method_thunk();
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void test_mono_call_method(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto cls_instance = assembly.new_class_instance(cls);
	auto method_thunk = cls_instance.get_method_thunk<void()>("Method");
	try
	{
		method_thunk();
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void test_mono_call_method2(mono::mono_domain& domain)
{
    log l(__func__);    
	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto cls_instance = assembly.new_class_instance(cls);
	auto method_thunk =
		cls_instance.get_method_thunk<std::string(std::string)>("MethodWithParameterAndReturnValue");
	auto result = method_thunk("test");
	assert("Return Value: test" == result);
}

void MyObject_CreateInternal(MonoObject* this_ptr)
{
	std::cout << "FROM C++ : MyObject created." << std::endl;
}

void MyObject_DestroyInternal(MonoObject* this_ptr)
{
	std::cout << "FROM C++ : MyObject deleted." << std::endl;
}

void MyObject_DoStuff(MonoObject* this_ptr, std::string value)
{
	std::cout << "FROM C++ : DoStuff was called with: " << value << std::endl;
}

std::string MyObject_ReturnAString(MonoObject* this_ptr, std::string value)
{
	std::cout << "FROM C++ : ReturnAString was called with: " << value << std::endl;
	return "The value: " + value;
}

void bind_mono()
{
	mono::add_internal_call("Ethereal.MyObject::CreateInternal",
									  mono_auto_wrap(MyObject_CreateInternal));
	mono::add_internal_call("Ethereal.MyObject::DestroyInternal",
									  mono_auto_wrap(MyObject_DestroyInternal));
	mono::add_internal_call("Ethereal.MyObject::DoStuff", 
                                      mono_auto_wrap(MyObject_DoStuff));
	mono::add_internal_call("Ethereal.MyObject::ReturnAString",
                            mono_auto_wrap(MyObject_ReturnAString));
}


}
