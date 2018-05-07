#include "tests.h"
#include "monopp/mono_assembly.h"
#include "monopp/mono_class.h"
#include "monopp/mono_class_instance.h"
#include "monopp/mono_method.h"
#include "monopp/mono_string.h"
#include "monort/managed_interface/mono_object_wrapper.h"
#include "monort/managed_interface/mono_pod_wrapper.h"

//References
//https://github.com/AvalonWot/xmono/blob/master/jni/lua-mono.cpp
//https://github.com/mono/mono/blob/master/samples/embed/test-invoke.c

struct vec2f
{
	float x;
	float y;
	int data = -1;
};

namespace mono
{
namespace managed_interface
{
struct vector2f
{
	float x;
	float y;
};
template <>
inline auto converter::convert(const vec2f& v) -> vector2f
{
	return vector2f{v.x, v.y};
}

template <>
inline auto converter::convert(const vector2f& v) -> vec2f
{
	return vec2f{v.x, v.y, 0};
}
}

register_basic_mono_converter_for_pod(vec2f, managed_interface::vector2f);
register_basic_mono_converter_for_wrapper(std::shared_ptr<vec2f>);
}

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

#define LOG_ENTRY log __l(__func__)

void test_mono_jit_get_assembly_fail(mono::mono_domain& domain)
{
	LOG_ENTRY;

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
	LOG_ENTRY;

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
	LOG_ENTRY;

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
	LOG_ENTRY;

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
	LOG_ENTRY;

	try
	{
		auto& assembly = domain.get_assembly("managed.dll");

		auto cls = assembly.get_class("ClassInstanceTest");
		auto obj = assembly.new_class_instance(cls);
		auto method_Method = obj.get_method("Method");
		auto method_MethodWithParameter = obj.get_method("MethodWithParameter", 1);
		auto method_MethodWithParameterAndReturnValue =
			obj.get_method("MethodWithParameterAndReturnValue", 2);
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		assert(false);
	}
}

void test_mono_jit_get_field(mono::mono_domain& domain)
{
	LOG_ENTRY;

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

void test_mono_jit_get_field_fail(mono::mono_domain& domain)
{
	LOG_ENTRY;

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
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_method<int(int)>("FunctionWithIntParam");
	const auto number = 1000;
	auto result = method_thunk(number);
	assert(number + 1337 == result);
}

void test_mono_call_thunk2(mono::mono_domain& domain)
{
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_method<void(float, int, float)>("VoidFunction");
	method_thunk(13.37f, 42, 9000.0f);
}

void test_mono_call_thunk3(mono::mono_domain& domain)
{
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_method<void(std::string)>("FunctionWithStringParam");
	method_thunk("Hello!");
}

void test_mono_call_thunk4(mono::mono_domain& domain)
{
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_method<std::string(std::string)>("StringReturnFunction");
	auto expected_string = std::string("Hello!");
	auto result = method_thunk(expected_string);
	assert(result == std::string("The string value was: " + expected_string));
}

void test_mono_call_thunk_with_exception(mono::mono_domain& domain)
{
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto method_thunk = cls.get_static_method<void()>("ExceptionFunction");
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
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto cls_instance = assembly.new_class_instance(cls);
	auto method_thunk = cls_instance.get_method<void()>("Method");
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
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto cls_instance = assembly.new_class_instance(cls);

	auto method_thunk =
		cls_instance.get_method<std::string(std::string, int)>("MethodWithParameterAndReturnValue");

	try
	{
		auto result = method_thunk("test", 5);
		assert("Return Value: test" == result);
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void test_mono_call_method3(mono::mono_domain& domain)
{
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto cls_instance = assembly.new_class_instance(cls);

	try
	{
		auto method_thunk = cls_instance.get_method<vec2f(vec2f)>("MethodPodAR");
		vec2f p{12, 15};
		auto result1 = method_thunk(p);
		assert(165.0f == result1.x && 7.0f == result1.y);
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void test_mono_call_method4(mono::mono_domain& domain)
{
	LOG_ENTRY;

	auto& assembly = domain.get_assembly("managed.dll");
	auto cls = assembly.get_class("ClassInstanceTest");
	auto cls_instance = assembly.new_class_instance(cls);

	try
	{
        using vec2f_ptr = std::shared_ptr<vec2f>;
        
        auto ptr = std::make_shared<vec2f>();
        ptr->x = 12;
        ptr->y = 15;
        
		auto method_thunk = cls_instance.get_method<void(vec2f_ptr)>("MethodPodARW");
		method_thunk(ptr);
	}
	catch(mono::mono_exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void MyObject_CreateInternal(MonoObject* this_ptr, float x, std::string v)
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

void MyVec_CreateInternal(MonoObject* this_ptr, float x, float y)
{
	std::cout << "FROM C++ : WrapperVector2f created." << std::endl;
	using vec2f_ptr = std::shared_ptr<vec2f>;
	auto p = std::make_shared<vec2f>();
	p->x = x;
	p->y = y;

	mono::managed_interface::mono_object_wrapper<vec2f_ptr>::create(this_ptr, p);
}

void bind_mono(mono::mono_domain& domain)
{
    auto& core_assembly = domain.get_assembly("managed_lib.dll");            
    auto& assembly = domain.get_assembly("managed.dll");
	mono::managed_interface::object::initialize_class_field(core_assembly);
    mono::managed_interface::object::register_internal_calls();

	mono::add_internal_call("Ethereal.MyObject::CreateInternal", mono_auto_wrap(MyObject_CreateInternal));
	mono::add_internal_call("Ethereal.MyObject::DestroyInternal", mono_auto_wrap(MyObject_DestroyInternal));
	mono::add_internal_call("Ethereal.MyObject::DoStuff", mono_auto_wrap(MyObject_DoStuff));
	mono::add_internal_call("Ethereal.MyObject::ReturnAString", mono_auto_wrap(MyObject_ReturnAString));
	mono::add_internal_call("Ethereal.WrapperVector2f::Create_WrapperVector2f",
							mono_auto_wrap(MyVec_CreateInternal));
}
}
